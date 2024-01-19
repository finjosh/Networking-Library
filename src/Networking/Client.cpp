#include "include/Networking/Client.hpp"

// TODO organize this

Client::Client(sf::IpAddress serverIP, unsigned short serverPort) 
{ 
    _serverIP = serverIP; 
    _serverPort = serverPort;  
}

Client::Client(unsigned short serverPort)
{
    _serverPort = serverPort;  
}

// ----------------

void Client::_initThreadFunctions()
{
    _updateFunc.setFunction(&_update, this);
}

void Client::_update(const float& deltaTime)
{
    if (this->isConnectionOpen()) 
    {
        _timeSinceLastPacket += deltaTime;
    }
    if (_timeSinceLastPacket >= _clientTimeoutTime) 
    { 
        this->Disconnect(); 
        return;
    }
}

Client::~Client()
{
    Disconnect();
}

bool Client::WasIncorrectPassword()
{ return _wrongPassword; }

void Client::setAndSendPassword(std::string password)
{ setPassword(password); this->sendPasswordToServer(); }

void Client::sendPasswordToServer()
{
    _wrongPassword = false;
    sf::Packet temp = this->PasswordPacket(_password);
        if (this->send(temp, _serverIP, _serverPort)) throw std::runtime_error("could not send password to host");
}

bool Client::ConnectToServer()
{
    _wrongPassword = false;

    sf::Packet connectionRequest = this->ConnectionRequestTemplate();

    if (_serverIP != sf::IpAddress::None)
    {
        if (!this->isReceivingPackets())
        {
            // delete(_sSource);
            // _sSource = nullptr;
            // _sSource = new std::stop_source;
            this->bind(Socket::AnyPort);
            startThreads(); //! needs to be started after the socket has been bind
            // _receive_thread = new std::jthread{thread_receive_packets, this, _sSource->get_token()};
        }
        // if (_update_thread == nullptr) _update_thread = new std::jthread{thread_update, this, _sSource->get_token()};
    }
    else
    {
        return false;
    }

    if (_serverIP == sf::IpAddress::LocalHost) _ip = sf::IpAddress::LocalHost.toInteger();
    else _ip = _ip;

    if (this->send(connectionRequest, _serverIP, _serverPort) != Socket::Done)
    {
        stopThreads();
        close();
        return false; 
    }

    return true;
}

void Client::setServerData(sf::IpAddress serverIP, unsigned short serverPort)
{
    setServerData(serverIP);
    setServerData(serverPort);  
}

void Client::setServerData(sf::IpAddress serverIP)
{
    if (_serverIP == "")
        _serverIP = sf::IpAddress::LocalHost;
    else
        _serverIP = serverIP; 
}

void Client::setServerData(Port port)
{
    _serverPort = port;
}

void Client::SendToServer(sf::Packet& packet)
{
    if (!_connectionOpen) return;
    _wrongPassword = false;
    if (this->send(packet, _serverIP, _serverPort))
        throw std::runtime_error("ERROR - could not send packet to the server");
}

void Client::_initPacketParsingFunctions()
{
    _parseData.setFunction(&_parseDataPacket, this);
    _parseConnectionConfirm.setFunction(&_parseConnectionConfirmPacket, this);
    _parseConnectionClose.setFunction(&_parseConnectionClosePacket, this);
    _parsePasswordRequest.setFunction(&_parsePasswordRequestPacket, this);
}

void Client::_parseDataPacket(sf::Packet* packet)
{
    _timeSinceLastPacket = 0.f;
    this->onDataReceived.invoke(*packet, _threadSafeEvents);
}

void Client::_parseConnectionClosePacket(sf::Packet* packet)
{
    _connectionOpen = false;
    _connectionTime = 0.f;
    Disconnect();
    this->onConnectionClose.invoke(_threadSafeEvents);
}

void Client::_parseConnectionConfirmPacket(sf::Packet* packet)
{
    _connectionOpen = true;
    _connectionTime = 0.f;
    (*packet) >> _ip; // getting the ip from the packet
    this->onConnectionOpen.invoke(_threadSafeEvents);
}

void Client::_parsePasswordRequestPacket(sf::Packet* packet)
{
    if (_needsPassword)
        _wrongPassword = true;
    else
        _wrongPassword = false;
    _needsPassword = true;
    this->onPasswordRequest.invoke(_threadSafeEvents);
}

void Client::Disconnect()
{
    if (this->isConnectionOpen())
    {
        sf::Packet close = this->ConnectionCloseTemplate();
        this->SendToServer(close);
        this->onConnectionClose.invoke(_threadSafeEvents);
    }
    else
    {
        return;
    }
 
    _connectionOpen = false;
    _needsPassword = false;
    _wrongPassword = false;
    this->setPassword("");
    _connectionTime = 0.f;
    _timeSinceLastPacket = 0.f;
    _serverIP = sf::IpAddress::None;

    stopThreads();
    close();
}

float Client::getTimeSinceLastPacket()
{ return _timeSinceLastPacket; }

sf::IpAddress Client::getServerIP()
{ return _serverIP; }

unsigned int Client::getServerPort()
{ return _serverPort; }
