#include "include/Networking/Server.hpp"

// TODO organize this
Server::Server(unsigned short port, bool passwordRequired)
{
    _port = port;
}

Server::~Server()
{
    closeConnection();
}

//* Protected Connection Functions
    
void Server::_resetConnectionData()
{
    SocketPlus::_resetConnectionData();
    _clientData.clear();
}

// ---------------------

//* Connection Functions

//* Pure Virtual Definitions

bool Server::tryOpenConnection()
{
    if (this->bind(_port))
        return false;

    _connectionOpen = true;
    startThreads();
    this->onConnectionOpen.invoke(_threadSafeEvents);
    return true;
}

void Server::closeConnection()
{
    if (!isConnectionOpen()) return;

    disconnectAllClients();

    _resetConnectionData();
    stopThreads();
    close();

    onConnectionClose.invoke(_threadSafeEvents);
}

// -------------------------

// ------------------------------------------------

void Server::requirePassword(bool requirePassword)
{ this->_needsPassword = requirePassword; if (!this->_needsPassword) setPassword(""); }

void Server::requirePassword(bool requirePassword, std::string password)
{ this->_needsPassword = requirePassword; setPassword(password); }

void Server::setPort(unsigned short port)
{
    _port = port;
}

void Server::sendToAll(sf::Packet& packet)
{
    for (auto& client: _clientData)
    {
        _sendTo(packet, sf::IpAddress((IP)client.second.id), client.second.port);
    }
}

void Server::sendTo(sf::Packet& packet, ID id)
{
    if (id != 0) 
    {
        ClientData* temp = &_clientData.find(id)->second;

        // if the ID is not a client anymore dont send packet
        if (temp == &_clientData.end()->second) return;
        
        _sendTo(packet, sf::IpAddress((IP)id), temp->port);
    }
}

bool Server::disconnectClient(ID id)
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

std::unordered_map<ID, ClientData>& Server::getClients()
{ return _clientData;}

ClientData Server::getClientData(std::pair<const ID, ClientData> client)
{ return client.second; }

sf::Uint32 Server::getNumberOfClients()
{ return (sf::Uint32)_clientData.size();}

float Server::getClientTimeSinceLastPacket(ID clientID)
{ return _clientData.find(clientID)->second.TimeSinceLastPacket; }

double Server::getClientConnectionTime(ID clientID)
{ return _clientData.find(clientID)->second.ConnectionTime; }

unsigned int Server::getClientPacketsPerSec(ID clientID)
{ return _clientData.find(clientID)->second.PacketsPerSecond; }

void Server::_initThreadFunctions() 
{
    _secondUpdateFunc.setFunction(&_secondUpdate, this);
    _updateFunc.setFunction(&_update, this);
}

void Server::_update(const float& deltaTime) 
{
    for (auto& clientData: _clientData)
    {
        clientData.second.TimeSinceLastPacket += deltaTime;
        if (clientData.second.TimeSinceLastPacket >= _clientTimeoutTime)
        {
            this->disconnectClient(clientData.first);
            this->onClientDisconnected.invoke(_threadSafeEvents);
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

void Server::_initPacketParsingFunctions()
{
    _parseData.setFunction(&_parseDataPacket, this);
    _parseConnectionRequest.setFunction(&_parseConnectionRequestPacket, this);
    _parseConnectionClose.setFunction(&_parseConnectionClosePacket, this);
    _parsePassword.setFunction(&_parsePasswordPacket, this);
}

void Server::_parseDataPacket(sf::Packet* packet, sf::IpAddress senderIP, PORT senderPort)
{
    // checking if the sender is a current client
    auto client = _clientData.find((ID)senderIP.toInteger()); 
    if (client != _clientData.end()) 
    {
        client->second.TimeSinceLastPacket = 0.0;
        client->second.PacketsSent++;
        this->onDataReceived.invoke((*packet), _threadSafeEvents);
    }
    // if the sender is not a current client add them if possible
    else
    {   
        if (!this->_needsPassword)
        {
            _clientData.insert({(ID)(senderIP.toInteger()), ClientData(senderPort, (ID)_ip)});

            sf::Packet Confirmation = this->ConnectionConfirmPacket(senderIP.toInteger());
            _sendTo(Confirmation, senderIP, senderPort);

            this->onClientConnected.invoke((ID)(senderIP.toInteger()), _threadSafeEvents);
        }
        else
        {
            sf::Packet needPassword = this->PasswordRequestPacket();
            _sendTo(needPassword, senderIP, senderPort);
        }
    }
}

void Server::_parseConnectionRequestPacket(sf::Packet* packet, sf::IpAddress senderIP, PORT senderPort)
{
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
    this->onClientConnected.invoke((ID)senderIP.toInteger(), _threadSafeEvents);
}

void Server::_parseConnectionClosePacket(sf::Packet* packet, sf::IpAddress senderIP)
{
    disconnectClient(senderIP.toInteger());
    this->onClientDisconnected.invoke((ID)senderIP.toInteger(), _threadSafeEvents);
}

void Server::_parsePasswordPacket(sf::Packet* packet, sf::IpAddress senderIP, PORT senderPort)
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
        this->onClientConnected.invoke((ID)ip, _threadSafeEvents);
    }
    else
    {
        sf::Packet passwordRequest;
        passwordRequest = this->PasswordRequestPacket();
        _sendTo(passwordRequest, senderIP, senderPort);
    }
}
