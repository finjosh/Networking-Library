#include "include/Networking/Client.hpp"

using namespace udp;

//* Initializer and Deconstructor

Client::Client(const sf::IpAddress& serverIP, const PORT& serverPort) 
{ 
    setServerData(serverIP, serverPort);
}

Client::Client(const PORT& serverPort)
{
    setServerData(serverPort);
}

Client::~Client()
{
    closeConnection();
}

// ------------------------------

//* Thread Functions

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

//* Pure Virtual Definitions

void Client::_initThreadFunctions()
{
    _updateFunc.setFunction(&_update, this);
}

// ------------------------

// -----------------

//* Protected Connection Functions

void Client::_resetConnectionData()
{
    Socket::_resetConnectionData();
    _wrongPassword = false;
    _timeSinceLastPacket = 0.f;
}

// ---------------------

//* Packet Parsing Functions

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

//* Pure Virtual Definition

void Client::_initPacketParsingFunctions()
{
    _parseData.setFunction(&_parseDataPacket, this);
    _parseConnectionConfirm.setFunction(&_parseConnectionConfirmPacket, this);
    _parseConnectionClose.setFunction(&_parseConnectionClosePacket, this);
    _parsePasswordRequest.setFunction(&_parsePasswordRequestPacket, this);
}

// -------------

// -------------------------

//* Connection Functions

bool Client::wasIncorrectPassword()
{ return _wrongPassword; }

void Client::setAndSendPassword(const std::string& password)
{ setPassword(password); this->sendPasswordToServer(); }

void Client::sendPasswordToServer()
{
    _wrongPassword = false;
    sf::Packet temp = this->PasswordPacket(_password);
    _sendTo(temp, getServerIP(), getServerPort());
}

void Client::setServerData(const sf::IpAddress& serverIP, const PORT& serverPort)
{
    setServerData(serverIP);
    setServerData(serverPort);  
}

void Client::setServerData(const sf::IpAddress& serverIP)
{
    if (getServerIP() == "")
        _serverIP = sf::IpAddress::LocalHost;
    else
        _serverIP = serverIP; 
    onServerIpChanged.invoke(getServerIP(), _threadSafeEvents);
}

void Client::setServerData(const PORT& port)
{
    _serverPort = port;
    onServerPortChanged.invoke(getServerPort(), _threadSafeEvents);
}

void Client::sendToServer(sf::Packet& packet)
{
    if (!_connectionOpen) return;
    _wrongPassword = false;
    _sendTo(packet, getServerIP(), getServerPort());
}

float Client::getTimeSinceLastPacket() const
{ return _timeSinceLastPacket; }

sf::IpAddress Client::getServerIP() const
{ return _serverIP; }

unsigned int Client::getServerPort() const
{ return _serverPort; }

// * Pure Virtual Definitions

bool Client::tryOpenConnection()
{
    _wrongPassword = false;

    sf::Packet connectionRequest = this->ConnectionRequestTemplate();

    if (getServerIP() != sf::IpAddress::None)
    {
        if (!this->isReceivingPackets())
        {
            this->bind(Socket::AnyPort);
            setPort(Socket::getLocalPort());
            startThreads(); //! needs to be called AFTER port binding
        }
    }
    else
    {
        return false;
    }

    // checking if connecting to localhost as ID will be different then
    if (getServerIP() == sf::IpAddress::LocalHost) _ip = sf::IpAddress::LocalHost.toInteger();
    else _ip = _ip;

    try
    {
        // if this fails socket did not open
        _sendTo(connectionRequest, getServerIP(), getServerPort());
    }
    catch(const std::exception& e)
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

// ---------------------------

// ---------------------
