#include "Networking/Server.hpp"

using namespace udp;

//* Initializer and Deconstructor

Server::Server(const unsigned short& port, const bool& passwordRequired)
{
    setPort(port);
}

Server::~Server()
{
    closeConnection();
}

// ------------------------------

//* Protected Connection Functions
    
void Server::_resetConnectionData()
{
    Socket::_resetConnectionData();
    _clientData.clear();
}

// ---------------------

//* Thread functions

void Server::_update(const float& deltaTime) 
{
    for (auto& clientData: _clientData)
    {
        clientData.second.TimeSinceLastPacket += deltaTime;
        if (clientData.second.TimeSinceLastPacket >= _clientTimeoutTime)
        {
            this->disconnectClient(clientData.first);
            this->onClientDisconnected.invoke(_threadSafeEvents, _overrideEvents);
        }
        clientData.second.ConnectionTime += deltaTime;
    }
}

void Server::_secondUpdate() 
{
    for (auto& clientData: _clientData)
    {
        clientData.second.PacketsPerSecond = clientData.second.PacketsSent;
        clientData.second.PacketsSent = 0;
    }
}

//* Pure Virtual Defintions

void Server::_initThreadFunctions() 
{
    _secondUpdateFunc.setFunction(&_secondUpdate, this);
    _updateFunc.setFunction(&_update, this);
}

// ------------------------

// -----------------

//* Packet Parsing Functions

void Server::_parseDataPacket(sf::Packet* packet, const sf::IpAddress& senderIP, const PORT& senderPort)
{
    // checking if the sender is a current client
    auto client = _clientData.find((ID)senderIP.toInteger()); 
    if (client != _clientData.end()) 
    {
        client->second.TimeSinceLastPacket = 0.0;
        client->second.PacketsSent++;
        this->onDataReceived.invoke((*packet), _threadSafeEvents, _overrideEvents);
    }
    // if the sender is not a current client add them if possible
    else
    {   
        if (!_needsPassword && _allowClientConnection)
        {
            _clientData.insert({(ID)(senderIP.toInteger()), ClientData(senderPort, (ID)_ip)});

            sf::Packet Confirmation = this->ConnectionConfirmPacket(senderIP.toInteger());
            _sendTo(Confirmation, senderIP, senderPort);

            this->onClientConnected.invoke((ID)(senderIP.toInteger()), _threadSafeEvents, _overrideEvents);
        }
        else
        {
            sf::Packet needPassword = this->PasswordRequestPacket();
            _sendTo(needPassword, senderIP, senderPort);
        }
    }
}

void Server::_parseConnectionRequestPacket(sf::Packet* packet, const sf::IpAddress& senderIP, const PORT& senderPort)
{
    if (!_allowClientConnection) return;
    if (this->_needsPassword)
    {
        sf::Packet needPassword = this->PasswordRequestPacket();
        _sendTo(needPassword, senderIP, senderPort);
        return; // dont want to confirm a connection if need password
    }
    else
    {
        IP ip = senderIP.toInteger();
        
        // checking if the client is not already connected
        if (_clientData.find(ip) == _clientData.end())
        {
            _clientData.insert({(ID)ip, ClientData(senderPort, (ID)ip)});
        }
        else // Client is already connected
        {
            return;
        }
    }

    sf::Packet Confirmation = this->ConnectionConfirmPacket(senderIP.toInteger());
    _sendTo(Confirmation, senderIP, senderPort);
    this->onClientConnected.invoke((ID)senderIP.toInteger(), _threadSafeEvents, _overrideEvents);
}

void Server::_parseConnectionClosePacket(sf::Packet* packet, const sf::IpAddress& senderIP)
{
    disconnectClient(senderIP.toInteger());
    this->onClientDisconnected.invoke((ID)senderIP.toInteger(), _threadSafeEvents, _overrideEvents);
}

void Server::_parsePasswordPacket(sf::Packet* packet, const sf::IpAddress& senderIP, const PORT& senderPort)
{
    std::string sentPassword;
    (*packet) >> sentPassword;
    IP ip = senderIP.toInteger();

    if (_password == sentPassword && _clientData.find(ip) == _clientData.end()) // if password is correct and client is not already connected
    {
        _clientData.insert({(ID)ip, ClientData(senderPort, (ID)ip)});
       
        // send confirmation as password was correct
        sf::Packet Confirmation = this->ConnectionConfirmPacket(senderIP.toInteger());
        _sendTo(Confirmation, senderIP, senderPort);
        this->onClientConnected.invoke((ID)ip, _threadSafeEvents, _overrideEvents);
    }
    else
    {
        sf::Packet passwordRequest;
        passwordRequest = this->PasswordRequestPacket();
        _sendTo(passwordRequest, senderIP, senderPort);
    }
}

//* Pure Virtual Definitions

void Server::_initPacketParsingFunctions()
{
    _parseData.setFunction(&_parseDataPacket, this);
    _parseConnectionRequest.setFunction(&_parseConnectionRequestPacket, this);
    _parseConnectionClose.setFunction(&_parseConnectionClosePacket, this);
    _parsePassword.setFunction(&_parsePasswordPacket, this);
}

// -------------------------

// --------------------------

//* Connection Functions

void Server::setPasswordRequired(const bool& requirePassword)
{ this->_needsPassword = requirePassword; if (!this->_needsPassword) setPassword(""); }

void Server::setPasswordRequired(const bool& requirePassword, const std::string& password)
{ this->_needsPassword = requirePassword; setPassword(password); }

bool Server::isPasswordRequired() const
{
    return _needsPassword;
}

void Server::sendToAll(sf::Packet& packet)
{
    for (auto& client: _clientData)
    {
        _sendTo(packet, sf::IpAddress((IP)client.second.id), client.second.port);
    }
}

void Server::sendTo(sf::Packet& packet, const ID& id)
{
    if (id != 0) 
    {
        ClientData* temp = &_clientData.find(id)->second;

        // if the ID is not a client anymore dont send packet
        if (temp == &_clientData.end()->second) return;

        _sendTo(packet, sf::IpAddress((IP)id), temp->port);
    }
}

bool Server::disconnectClient(const ID& id)
{
    if (_clientData.find(id) != _clientData.end())
    {
        sf::Packet RemoveClient = this->ConnectionCloseTemplate();
        sendTo(RemoveClient, id);
        _clientData.erase(id);
        return true;
    }
    else return false;
}

void Server::disconnectAllClients()
{
    sf::Packet RemoveClient = this->ConnectionCloseTemplate();

    this->sendToAll(RemoveClient);

    _clientData.clear();
}

const std::unordered_map<ID, ClientData>& Server::getClients() const
{ return _clientData;}

ClientData Server::getClientData(const std::pair<const ID, ClientData>& client)
{ return client.second; }

sf::Uint32 Server::getClientsSize() const
{ return (sf::Uint32)_clientData.size();}

float Server::getClientTimeSinceLastPacket(const ID& clientID) const
{ return _clientData.find(clientID)->second.TimeSinceLastPacket; }

double Server::getClientConnectionTime(const ID& clientID) const
{ return _clientData.find(clientID)->second.ConnectionTime; }

unsigned int Server::getClientPacketsPerSec(const ID& clientID) const
{ return _clientData.find(clientID)->second.PacketsPerSecond; }

void Server::allowClientConnection(const bool& allowed)
{
    _allowClientConnection = allowed;
}

bool Server::isClientConnectionAllowed()
{
    return _allowClientConnection;
}

//* Pure Virtual Definitions

bool Server::tryOpenConnection()
{
    if (this->bind(getPort()))
        return false;

    _connectionOpen = true;
    startThreads();
    this->onConnectionOpen.invoke(_threadSafeEvents, _overrideEvents);
    return true;
}

void Server::closeConnection()
{
    if (!isConnectionOpen()) return;

    disconnectAllClients();

    _resetConnectionData();
    stopThreads();
    close();

    onConnectionClose.invoke(_threadSafeEvents, _overrideEvents);
}

// -------------------------

// ---------------------
