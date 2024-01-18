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
    if (_sSource != nullptr) _sSource->request_stop();
    if (_receive_thread != nullptr) 
    {
        _receive_thread->detach();
    }
    if (_update_thread != nullptr)
    {
        _update_thread->detach();
    }
    delete(_receive_thread);
    delete(_update_thread);
    delete(_sSource);
    this->close();
}

// ------------------------------

//* Protected Thread Functions

void SocketPlus::thread_update(std::stop_token sToken)
{
    UpdateLimiter updateLimit(_socketUpdateRate);

    sf::Clock deltaClock;
    float deltaTime;
    float secondTime;
    
    while (!sToken.stop_requested())
    {
        // TODO do this with an event
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
            _secondUpdate.invoke();
            secondTime = 0.f;
        }

        // calling the fixed update function
        _update.invoke(deltaTime);
        
        // if we are sending packets call the sending function
        if (_sendingPackets) 
            _packetSendFunction.invoke();
        
        updateLimit.wait();
    }
}

// ---------------------------

//* Public Thread functions

void SocketPlus::startThreads()
{
    this->initThreadFunctions();

    if (_receive_thread == nullptr)
    {
        if (_sSource != nullptr) delete(_sSource);
        _sSource = new std::stop_source;
        _receive_thread = new std::jthread{thread_receive_packets, this, _sSource->get_token()};
    }
    if (_update_thread == nullptr) _update_thread = new std::jthread{thread_update, this, _sSource->get_token()};
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

bool SocketPlus::isConnectionRequest(sf::Packet packet)
{
    int temp;
    
    if ((packet >> temp) && temp == PacketType::ConnectionClose)
        return true;

    // returning false if this packet is not a connection close
    return false;
}

bool SocketPlus::isConnectionClose(sf::Packet packet)
{
    int temp;
    
    if ((packet >> temp) && temp == PacketType::ConnectionClose)
        return true;


    // returning false if this packet is not a connection close
    return false;
}

bool SocketPlus::isData(sf::Packet packet)
{
    int temp;
    
    if ((packet >> temp) && temp == PacketType::ConnectionClose)
        return true;

    // returning false if this packet is not a connection close
    return false;
}

bool SocketPlus::isConnectionConfirm(sf::Packet packet)
{
    int temp;
    
    if ((packet >> temp) && temp == PacketType::ConnectionConfirm)
        return true;

    // returning false if this packet is not a connection confirm
    return false;
}

bool SocketPlus::isPasswordRequest(sf::Packet packet)
{
    int temp;
    
    if ((packet >> temp) && temp == PacketType::RequestPassword)
        return true;

    // returning false if this packet is not a connection confirm
    return false;
}

bool SocketPlus::isPassword(sf::Packet packet)
{
    int temp;
    
    if ((packet >> temp) && temp == PacketType::Password)
        return true;

    // returning false if this packet is not a connection confirm
    return false;
}

bool SocketPlus::isWrongPassword(sf::Packet packet)
{
    int temp;
    
    if ((packet >> temp) && temp == PacketType::WrongPassword)
        return true;
    // returning false if this packet is not a connection confirm
    return false;
}

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

// bool SocketPlus::isThreadSafeEvents()
// {
//     return _threadSafeEvents;
// }

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
    out << PacketType::RequestPassword;
    return out;
}

sf::Packet SocketPlus::PasswordPacket(std::string password)
{
    sf::Packet out;
    out << PacketType::Password;
    out << password;
    return out;
}

sf::Packet SocketPlus::WrongPasswordPacket()
{
    sf::Packet out;
    out << PacketType::WrongPassword;
    return out;
}

// -------------------
