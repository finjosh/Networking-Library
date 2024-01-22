#include "include/Networking/SocketPlus.hpp"

//* initializer and deconstructor

SocketPlus::SocketPlus()
{
    _ip = sf::IpAddress::getPublicAddress().toInteger();
    _port = this->getLocalPort();
    // onPortChanged.invoke(_threadSafeEvents);
}

SocketPlus::~SocketPlus()
{   
    stopThreads();
    this->close();
}

// ------------------------------

//* Protected Thread Functions

void SocketPlus::_thread_receive_packets(std::stop_token sToken)
{
    sf::Packet packet;
    sf::IpAddress senderIP;
    unsigned short senderPort;

    while (!sToken.stop_requested()) {
        Status receiveStatus = this->receive(packet, senderIP, senderPort);
        if (receiveStatus == sf::Socket::Error)
        {
            if (sToken.stop_requested()) break;
            throw std::runtime_error("ERROR - receiving packet");
        }
        else if (receiveStatus != sf::Socket::Done)
        {
            std::cerr << "Error receiving packet: " << std::to_string(receiveStatus) << std::endl;
            packet.clear();
            continue;
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
            std::cerr << "Packet Type not given... Value received: " << std::to_string(packetType) << std::endl;
            break;
        }

        packet.clear();
    }
}

void SocketPlus::_thread_update(std::stop_token sToken)
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

void SocketPlus::_resetConnectionData()
{
    _needsPassword = false;
    setPassword("");
    _connectionOpen = false;
    _connectionTime = 0.f;
}

// -------------------------------

//* Socket Functions

void SocketPlus::_sendTo(sf::Packet& packet, const sf::IpAddress& ip, const PORT& port)
{
    if (sf::UdpSocket::send(packet, sf::IpAddress(_ip), _port))
        throw std::runtime_error("ERROR - Could not send packet (_sendTo Function)");
}

// -----------------

//* Public Thread functions

void SocketPlus::startThreads()
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

void SocketPlus::stopThreads()
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

ID SocketPlus::getID()
{ return (ID)_ip; }

sf::IpAddress SocketPlus::getIP()
{ return sf::IpAddress(_ip); }

sf::IpAddress SocketPlus::getLocalIP()
{ return sf::IpAddress::getLocalAddress(); }

IP SocketPlus::getIP_I()
{ return _ip; }

double SocketPlus::getConnectionTime()
{ return _connectionTime; }

double SocketPlus::getOpenTime()
{ return _connectionTime; }

unsigned int SocketPlus::getUpdateInterval()
{ return _socketUpdateRate; }

unsigned int SocketPlus::getPort()
{ return _port; }

int SocketPlus::getClientTimeout()
{ return _clientTimeoutTime; }

std::string SocketPlus::getPassword()
{
    return _password;
}

const funcHelper::func<void>& SocketPlus::getPacketSendFunction()
{
    return _packetSendFunction;
}

// -------

//* Setters

void SocketPlus::setUpdateInterval(unsigned int interval)
{ 
    this->_socketUpdateRate = interval;
    onUpdateRateChanged.invoke(interval, _threadSafeEvents);
}

void SocketPlus::sendingPackets(bool sendPackets)
{ this->_sendingPackets = sendPackets; }

void SocketPlus::setPassword(std::string password)
{ 
    this->_password = password; 
    onPasswordChanged.invoke(password, _threadSafeEvents);
}

void SocketPlus::setClientTimeout(const int& timeout)
{ 
    _clientTimeoutTime = timeout; 
    onClientTimeoutChanged.invoke(timeout, _threadSafeEvents);
}

bool SocketPlus::setPacketSendFunction(funcHelper::func<void> packetSendFunction)
{
    if (this->isConnectionOpen()) return false;
    _packetSendFunction = packetSendFunction;
    onPacketSendChanged.invoke(_threadSafeEvents);
    return true;
}

// --------

//* Boolean question Functions

bool SocketPlus::isConnectionOpen()
{ return _connectionOpen; }

bool SocketPlus::isReceivingPackets()
{
    return (_receive_thread != nullptr);
}

bool SocketPlus::isSendingPackets()
{ return _sendingPackets && _connectionOpen; }

bool SocketPlus::NeedsPassword()
{ return this->_needsPassword; }

bool SocketPlus::isValidIpAddress(sf::IpAddress ipAddress)
{
    if (ipAddress != sf::IpAddress::None) return true;
    else return false;
}

bool SocketPlus::isValidIpAddress(sf::Uint32 ipAddress)
{
    if (sf::IpAddress(ipAddress) != sf::IpAddress::None) return true;
    else return false;
}

// #include "include/Utils/Stopwatch.hpp"

bool SocketPlus::isValidIpAddress(std::string ipAddress)
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

sf::Packet SocketPlus::ConnectionCloseTemplate()
{
    sf::Packet out;
    out << PacketType::ConnectionClose;
    return out;
}

sf::Packet SocketPlus::ConnectionRequestTemplate()
{
    sf::Packet out;
    out << PacketType::ConnectionRequest;
    return out;
}

sf::Packet SocketPlus::DataPacketTemplate()
{
    sf::Packet out;
    out << PacketType::Data;
    return out;
}

sf::Packet SocketPlus::ConnectionConfirmPacket(sf::Uint32 id)
{
    sf::Packet out;
    out << PacketType::ConnectionConfirm;
    out << id;
    return out;
}

sf::Packet SocketPlus::PasswordRequestPacket()
{
    sf::Packet out;
    out << PacketType::PasswordRequest;
    return out;
}

sf::Packet SocketPlus::PasswordPacket(std::string password)
{
    sf::Packet out;
    out << PacketType::Password;
    out << password;
    return out;
}

// -------------------
