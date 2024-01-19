#include "include/Networking/Server.hpp"

// TODO organize this
Server::Server(unsigned short port, bool passwordRequired)
{
    _port = port;
}

Server::~Server()
{
    CloseServer();
}

void Server::RequirePassword(bool requirePassword)
{ this->_needsPassword = requirePassword; if (!this->_needsPassword) setPassword(""); }

void Server::RequirePassword(bool requirePassword, std::string password)
{ this->_needsPassword = requirePassword; setPassword(password); }

bool Server::openServer()
{
    if (this->bind(_port))
        return false;

    _connectionOpen = true;
    startThreads();
    this->onConnectionOpen.invoke(_threadSafeEvents);
    return true;
}

void Server::CloseServer()
{
    if (!this->isConnectionOpen()) return;

    sf::Packet closeConnection = this->ConnectionCloseTemplate();
    this->SendToAll(closeConnection);

    this->stopThreads();
    this->close();

    if (_connectionOpen)
        this->onConnectionClose.invoke(_threadSafeEvents);

    _needsPassword = false;
    this->setPassword("");
    _connectionOpen = false;
    // _lastID = 0;
    this->deletedClientIDs.clear();
    this->newClientIDs.clear();
    // this->DataPackets.clear();
    _clientData.clear();
    _connectionTime = 0.0;
}

void Server::setPort(unsigned short port)
{
    _port = port;
}

void Server::SendToAll(sf::Packet& packet)
{
    for (auto& c: _clientData)
    {
        if (this->send(packet, sf::IpAddress((IP)c.first), c.second.port))
            throw std::runtime_error("ERROR - Server.h (Could not send packet to all client data)");
    }
}

void Server::SendTo(sf::Packet& packet, ID id)
{
    if (id != 0) 
    {
        ClientData* temp = &_clientData.find(id)->second;

        // if the ID is not a client anymore dont send packet
        if (temp == &_clientData.end()->second) return;
        
        if (this->send(packet, sf::IpAddress((IP)id), temp->port))
            throw std::runtime_error("ERROR - Could not send packet to client");
    }
}

// TODO move the disconnect event to these functions
bool Server::disconnectClient(ID id)
{
    if (_clientData.find(id) != _clientData.end())
    {
        sf::Packet RemoveClient = this->ConnectionCloseTemplate();
        SendTo(RemoveClient, id);
        _clientData.erase(id);
        this->deletedClientIDs.push_back(id);
        return true;
    }
    else return false;
}

// TODO move the disconnect event to these functions
void Server::disconnectAllClients()
{
    sf::Packet RemoveClient = this->ConnectionCloseTemplate();

    this->SendToAll(RemoveClient);

    for (auto& c: _clientData)
        this->deletedClientIDs.push_back(c.first);
    _clientData.clear();
}

std::unordered_map<ID, ClientData>& Server::getClients()
{ return _clientData;}

ClientData Server::getClientData(std::pair<const ID, ClientData> client)
{ return client.second; }

sf::Uint32 Server::NumberOfClients()
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

void Server::_parseDataPacket(sf::Packet* packet, sf::IpAddress senderIP, Port senderPort)
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
            if (this->send(Confirmation, senderIP, senderPort))
                throw std::runtime_error("ERROR - could not send connection confirmation");
            this->onClientConnected.invoke(_threadSafeEvents);
        }
        else
        {
            sf::Packet needPassword = this->PasswordRequestPacket();
            if (this->send(needPassword, senderIP, senderPort) != Socket::Done) 
                throw std::runtime_error("ERROR - could not send password request");
        }
    }
}

void Server::_parseConnectionRequestPacket(sf::Packet* packet, sf::IpAddress senderIP, Port senderPort)
{
    if (this->_needsPassword)
    {
        sf::Packet needPassword = this->PasswordRequestPacket();
        if (this->send(needPassword, senderIP, senderPort) != Socket::Done) 
            throw std::runtime_error("ERROR - could not send password request");
        return; // dont want to confirm a connection if need password
    }
    else
    {
        IP ip = senderIP.toInteger();
        
        // checking if the client is not already connected
        if (_clientData.find(ip) == _clientData.end())
        {
            _clientData.insert({(ID)ip, ClientData(senderPort, (ID)ip)});
            this->newClientIDs.push_back((ID)ip); 
        }
        else // Client is already connected
        {
            return;
        }
    }

    sf::Packet Confirmation = this->ConnectionConfirmPacket(senderIP.toInteger());
    if (this->send(Confirmation, senderIP, senderPort))
        throw std::runtime_error("ERROR - could not send connection confirmation");
    this->onClientConnected.invoke((ID)senderIP.toInteger(), _threadSafeEvents);
}

void Server::_parseConnectionClosePacket(sf::Packet* packet, sf::IpAddress senderIP)
{
    disconnectClient(senderIP.toInteger());
    this->onClientDisconnected.invoke((ID)senderIP.toInteger(), _threadSafeEvents);
}

void Server::_parsePasswordPacket(sf::Packet* packet, sf::IpAddress senderIP, Port senderPort)
{
    std::string sentPassword;
    (*packet) >> sentPassword;
    IP ip = senderIP.toInteger();

    if (_password == sentPassword && _clientData.find(ip) == _clientData.end()) // if password is correct and client is not already connected
    {
        _clientData.insert({(ID)ip, ClientData(senderPort, (ID)ip)});
        this->newClientIDs.push_back((ID)ip); // TODO remove this
       
        // send confirmation as password was correct
        sf::Packet Confirmation = this->ConnectionConfirmPacket(senderIP.toInteger());
        if (this->send(Confirmation, senderIP, senderPort))
            throw std::runtime_error("ERROR - could not send ID Assign packet");
        this->onClientConnected.invoke(_threadSafeEvents);
    }
    else
    {
        sf::Packet passwordRequest;
        passwordRequest = this->PasswordRequestPacket();
        if (this->send(passwordRequest, senderIP, senderPort)) 
            throw std::runtime_error("ERROR - could not send request for password");
    }
}
