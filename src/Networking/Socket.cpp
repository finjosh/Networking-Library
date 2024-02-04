#include "Networking/Socket.hpp"

using namespace udp;

//* initializer and deconstructor

Socket::Socket()
{
    _ip = sf::IpAddress::getPublicAddress().toInteger();
    setPort(getLocalPort());
}

Socket::~Socket()
{   
    stopThreads();
    close();
}

// ------------------------------

//* Protected Thread Functions

void Socket::_thread_receive_packets(std::stop_token sToken)
{
    sf::Packet packet;
    sf::IpAddress senderIP;
    unsigned short senderPort;

    while (!sToken.stop_requested()) {
        Status receiveStatus = this->receive(packet, senderIP, senderPort);
        if (receiveStatus == sf::Socket::Error)
        {
            if (sToken.stop_requested()) break;
            throw std::runtime_error("Receiving packet: " + std::to_string(receiveStatus));
        }

        int packetType;
        packet >> packetType;

        switch (packetType)
        {
        case PacketType::Data:
            if (packet.endOfPacket()) continue;
            _parseData.invoke(&packet, senderIP, senderPort);
            break;
        
        case PacketType::ConnectionRequest:
            _parseConnectionRequest.invoke(&packet, senderIP, senderPort);
            break;

        case PacketType::ConnectionClose:
            _parseConnectionClose.invoke(&packet, senderIP, senderPort);
            break;

        case PacketType::ConnectionConfirm:
            _parseConnectionConfirm.invoke(&packet, senderIP, senderPort);
            break;

        case PacketType::PasswordRequest:
            _parsePasswordRequest.invoke(&packet, senderIP, senderPort);
            break;

        case PacketType::Password:
            _parsePassword.invoke(&packet, senderIP, senderPort);
            break;

        default:
            throw std::runtime_error("Packet Type not given... Value received: " + std::to_string(packetType)); // TODO dont throw error just warn program
            break;
        }

        packet.clear();
    }
}

void Socket::_thread_update(std::stop_token sToken)
{
    UpdateLimiter updateLimit(_socketUpdateRate);

    sf::Clock deltaClock;
    float deltaTime;
    float secondTime;
    
    while (!sToken.stop_requested())
    {
        if (updateLimit.getUpdateLimit() != _socketUpdateRate)
        {
            updateLimit.updateLimit(_socketUpdateRate);
        }

        deltaTime = deltaClock.restart().asSeconds();
        secondTime += deltaTime;
        _connectionTime += deltaTime;
        
        // checking if second update should be called
        if (secondTime >= 1.f)
        {
            _secondUpdateFunc.invoke();
            secondTime = 0.f;
        }

        // calling the fixed update function
        _updateFunc.invoke(deltaTime);
        
        // if we are sending packets call the sending function
        if (_sendingPackets) 
            _packetSendFunction.invoke();
        
        updateLimit.wait();
    }
}

// ---------------------------

//* Protected Connection Functions

void Socket::_resetConnectionData()
{
    _needsPassword = false;
    setPassword("");
    _connectionOpen = false;
    _connectionTime = 0.f;
}

// -------------------------------

//* Socket Functions

void Socket::_sendTo(sf::Packet& packet, const sf::IpAddress& ip, const PORT& port)
{
    if (sf::UdpSocket::send(packet, sf::IpAddress(ip), port))
        throw std::runtime_error("Could not send packet (Socket::_sendTo Function)");
}

// -----------------

//* Public Thread functions

void Socket::startThreads()
{
    this->_initThreadFunctions();
    this->_initPacketParsingFunctions();

    if (_receive_thread == nullptr)
    {
        if (_sSource != nullptr) delete(_sSource);
        _sSource = new std::stop_source;
        _receive_thread = new std::jthread{_thread_receive_packets, this, _sSource->get_token()};
    }
    if (_update_thread == nullptr) _update_thread = new std::jthread{_thread_update, this, _sSource->get_token()};
}

void Socket::stopThreads()
{
    if (_sSource == nullptr) return;
    _sSource->request_stop();
    if (_update_thread != nullptr)
    {
        _update_thread->detach();
        delete(_update_thread);
        _update_thread = nullptr;
    }
    if (_receive_thread != nullptr)
    {
        // Sending a packet to its self so the receive thread can continue execution and exit
        sf::Packet temp = DataPacketTemplate();
        _sendTo(temp, sf::IpAddress{_ip}, _port);
        
        _receive_thread->detach();
        delete(_receive_thread);
        _receive_thread = nullptr;
    }
    delete(_sSource);
    _sSource = nullptr;
}

// ------------------------

//* Getter

ID Socket::getID() const
{ return (ID)_ip; }

sf::IpAddress Socket::getIP() const
{ return sf::IpAddress(_ip); }

sf::IpAddress Socket::getLocalIP() const
{ return sf::IpAddress::getLocalAddress(); }

IP Socket::getIP_I() const
{ return _ip; }

double Socket::getConnectionTime() const
{ return _connectionTime; }

double Socket::getOpenTime() const
{ return _connectionTime; }

unsigned int Socket::getUpdateInterval() const
{ return _socketUpdateRate; }

unsigned int Socket::getPort() const
{ return _port; }

int Socket::getClientTimeout() const
{ return _clientTimeoutTime; }

std::string Socket::getPassword() const
{
    return _password;
}

const funcHelper::func<void>& Socket::getPacketSendFunction() const
{
    return _packetSendFunction;
}

// -------

//* Setters

void Socket::setUpdateInterval(const unsigned int& interval)
{ 
    this->_socketUpdateRate = interval;
    onUpdateRateChanged.invoke(interval, _threadSafeEvents);
}

void Socket::sendingPackets(const bool& sendPackets)
{ this->_sendingPackets = sendPackets; }

void Socket::setPassword(const std::string& password)
{ 
    this->_password = password; 
    onPasswordChanged.invoke(password, _threadSafeEvents);
}

void Socket::setClientTimeout(const int& timeout)
{ 
    _clientTimeoutTime = timeout; 
    onClientTimeoutChanged.invoke(timeout, _threadSafeEvents);
}

bool Socket::setPacketSendFunction(const funcHelper::func<void>& packetSendFunction)
{
    if (this->isConnectionOpen()) return false;
    _packetSendFunction = packetSendFunction;
    onPacketSendChanged.invoke(_threadSafeEvents);
    return true;
}

void Socket::setPort(const PORT& port)
{
    _port = port;
    onPortChanged.invoke(_port, _threadSafeEvents);
}

// --------

//* Boolean question Functions

bool Socket::isConnectionOpen() const
{ return _connectionOpen; }

bool Socket::isReceivingPackets() const
{
    return (_receive_thread != nullptr);
}

bool Socket::isSendingPackets() const
{ return _sendingPackets && _connectionOpen; }

bool Socket::NeedsPassword() const
{ return this->_needsPassword; }

bool Socket::isValidIpAddress(const sf::IpAddress& ipAddress)
{
    if (ipAddress != sf::IpAddress::None) return true;
    else return false;
}

bool Socket::isValidIpAddress(const sf::Uint32& ipAddress)
{
    if (sf::IpAddress(ipAddress) != sf::IpAddress::None) return true;
    else return false;
}

// #include "include/Utils/Stopwatch.hpp"

bool Socket::isValidIpAddress(const std::string& ipAddress)
{
    // timer::Stopwatch timer;
    if (sf::IpAddress(ipAddress) != sf::IpAddress::None) 
    {
        return true;
    }
    else 
    {
        // std::cout << std::to_string(timer.lap()/1000.f) << std::endl;
        return false;
    }
}

// ---------------------------

//* Template Functions

sf::Packet Socket::ConnectionCloseTemplate()
{
    sf::Packet out;
    out << PacketType::ConnectionClose;
    return out;
}

sf::Packet Socket::ConnectionRequestTemplate()
{
    sf::Packet out;
    out << PacketType::ConnectionRequest;
    return out;
}

sf::Packet Socket::DataPacketTemplate()
{
    sf::Packet out;
    out << PacketType::Data;
    return out;
}

sf::Packet Socket::ConnectionConfirmPacket(const sf::Uint32& id)
{
    sf::Packet out;
    out << PacketType::ConnectionConfirm;
    out << id;
    return out;
}

sf::Packet Socket::PasswordRequestPacket()
{
    sf::Packet out;
    out << PacketType::PasswordRequest;
    return out;
}

sf::Packet Socket::PasswordPacket(const std::string& password)
{
    sf::Packet out;
    out << PacketType::Password;
    out << password;
    return out;
}

// -------------------