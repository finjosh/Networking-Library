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

//* Protected Connection Functions
    
void Client::_resetConnectionData()
{
    SocketPlus::_resetConnectionData();
    _serverIP = sf::IpAddress::None;
    _wrongPassword = false;
    _timeSinceLastPacket = 0.f;
}
    
// ---------------------

//* Connection Functions

// * Pure Virtual Definitions

bool Client::tryOpenConnection()
{
    _wrongPassword = false;

    sf::Packet connectionRequest = this->ConnectionRequestTemplate();

    if (_serverIP != sf::IpAddress::None)
    {
        if (!this->isReceivingPackets())
        {
            this->bind(Socket::AnyPort);
            startThreads(); //! needs to be called AFTER port binding
        }
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

void Client::closeConnection()
{
    if (this->isConnectionOpen())
    {
        sf::Packet close = this->ConnectionCloseTemplate();
        this->sendToServer(close);
        this->onConnectionClose.invoke(_threadSafeEvents);
    }
    else
    {
        return;
    }
 
    _resetConnectionData();
    stopThreads();
    close();
}

// --------------------------

// ------------------------------------------------

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
        this->closeConnection(); 
        return;
    }
}

Client::~Client()
{
    closeConnection();
}

bool Client::wasIncorrectPassword()
{ return _wrongPassword; }

void Client::setAndSendPassword(std::string password)
{ setPassword(password); this->sendPasswordToServer(); }

void Client::sendPasswordToServer()
{
    _wrongPassword = false;
    sf::Packet temp = this->PasswordPacket(_password);
        if (this->send(temp, _serverIP, _serverPort)) throw std::runtime_error("could not send password to host");
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

void Client::sendToServer(sf::Packet& packet)
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
    closeConnection();
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

float Client::getTimeSinceLastPacket()
{ return _timeSinceLastPacket; }

sf::IpAddress Client::getServerIP()
{ return _serverIP; }

unsigned int Client::getServerPort()
{ return _serverPort; }
