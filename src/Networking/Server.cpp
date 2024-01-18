#include "include/Networking/Server.hpp"

// TODO organize this
Server::Server(unsigned short port, bool passwordRequired)
{
    _port = port;
}

Server::~Server()
{}

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

bool Server::isConnectionRequest(sf::Packet& packet, sf::IpAddress senderIPAddress, unsigned short senderPort)
{
    int temp;

    // making a copy of the packet in case that it is not a connection request
    // if it is a connection request we then remove that int from the packet to make the main script simpler and no need to remember to remove it
    sf::Packet c_packet = packet;
    
    if ((c_packet >> temp) && temp == PacketType::ConnectionRequest)
    {
        packet >> temp; // removing the packet type infomation from the packet
        if (this->_needsPassword)
        {
            sf::Packet needPassword = this->PasswordRequestPacket();
            if (this->send(needPassword, senderIPAddress, senderPort) != Socket::Done) 
                throw std::runtime_error("ERROR - could not send password request");
        }
        else
        {
            IP ip = senderIPAddress.toInteger();
            
            // checking if the connection is not from a client that is still in the clientData
            if (_clientData.find(ip) == _clientData.end())
            {
                _clientData.insert({(ID)ip, ClientData(senderPort, (ID)ip)});
                // _lastID = (ID)ip;
                this->newClientIDs.push_back((ID)ip);
                
                return true;
            }
            else // if the client IP is already connected to the server find the ID and send to the client
            {
                packet >> temp; // removing the packet type infomation from the packet
                return true;
            }
        }
    }

    // returning false if this packet is not a connection
    return false;
}

bool Server::isConnectionClose(sf::Packet& packet, sf::IpAddress senderIPAddress, unsigned short senderPort)
{
    int temp;

    // making a copy of the packet in case that it is not a connection request
    // if it is a connection request we then remove that int from the packet to make the main script simpler and no need to remember to remove it
    sf::Packet c_packet = packet;
    
    if ((c_packet >> temp) && temp == PacketType::ConnectionClose)
    {
        packet >> temp; // removing the packet type infomation from the packet

        disconnectClient(senderIPAddress.toInteger());

        return true;
    }

    // returning false if this packet is not a connection close
    return false;
}

bool Server::isPassword(sf::Packet& packet, sf::IpAddress senderIPAddress, unsigned short senderPort)
{
    int temp;

    // making a copy of the packet in case that it is not a connection request
    // if it is a connection request we then remove that int from the packet to make the main script simpler and no need to remember to remove it
    sf::Packet c_packet = packet;
    
    // if this is the right packet type
    if ((c_packet >> temp) && temp == PacketType::Password)
    {
        packet >> temp; // removing the packet type infomation from the packet
        std::string sentPassword;
        packet >> sentPassword;

        if (_password == sentPassword)
        {
            IP ip = senderIPAddress.toInteger();
            
            // checking if the connection is not from a client that is still in the clientData
            if (_clientData.find(ip) == _clientData.end())
            {
                _clientData.insert({(ID)ip, ClientData(senderPort, (ID)ip)});
                // _lastID = (ID)ip;
                this->newClientIDs.push_back((ID)ip);
                
                return true;
            }
            else // if the client IP is already connected to the server find the ID and send to the client
            {
                packet >> temp; // removing the packet type infomation from the packet
                return true;
            }
        }
        else
        {
            sf::Packet wrongPassword;
            wrongPassword = this->WrongPasswordPacket();
            if (this->send(wrongPassword, senderIPAddress, senderPort)) 
                throw std::runtime_error("ERROR - could not send request for password");
        }
    }

    return false;
}

bool Server::isData(sf::Packet& packet)
{
    int temp;

    // making a copy of the packet in case that it is not a connection request
    // if it is a connection request we then remove that int from the packet to make the main script simpler and no need to remember to remove it
    sf::Packet c_packet = packet;
    
    if ((c_packet >> temp) && temp == PacketType::Data)
    {
        packet >> temp; // removing the packet type infomation from the packet
        return true;
    }

    // returning false if this packet is not a data packet
    return false;
}

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

void Server::disconnectAllClients()
{
    sf::Packet RemoveClient = this->ConnectionCloseTemplate();

    this->SendToAll(RemoveClient);

    for (auto& c: _clientData)
        this->deletedClientIDs.push_back(c.first);
    _clientData.clear();
}

void Server::thread_receive_packets(std::stop_token stoken)
{
    sf::Packet packet;
    sf::IpAddress senderIP;
    unsigned short senderPort;

    while (!stoken.stop_requested()) {
        Status receiveStatus = this->receive(packet, senderIP, senderPort);
        if (receiveStatus == sf::Socket::Error)
        {
            if (stoken.stop_requested()) break;
            throw std::runtime_error("ERROR - receiving packet");
            // restarting the socket
            this->unbind();
            this->close();
            this->bind(_port);
            packet.clear();
            // skip over rest of loop to prevent crash
            continue;
        }
        // TODO make this better (I dont know if this happens every frame forever but the disconnected status keeps on popping up)
        else if (receiveStatus != sf::Socket::Done)
        {
            packet.clear();
            continue;
        }

        if (this->isData(packet))
        { 
            // checking if the sender is a current client
            auto client = _clientData.find((ID)senderIP.toInteger()); 
            if (client != _clientData.end()) 
            {
                // this->DataPackets.push_back(DataPacket(packet)); // TODO remove this after ensuring that the event works
                client->second.TimeSinceLastPacket = 0.0;
                client->second.PacketsSent++;
                this->onDataReceived.invoke(packet, _threadSafeEvents);
            }
            // if the sender is not a current client add them
            else
            {   
                if (!this->_needsPassword)
                {
                    // _lastID = senderIP.toInteger();
                    // newClientIDs.push_back(_lastID);
                    _clientData.insert({(ID)(senderIP.toInteger()), ClientData(senderPort, (ID)_ip)});

                    // this->DataPackets.push_back(DataPacket(packet));
                    sf::Packet Confirmation = this->ConnectionConfirmPacket(senderIP.toInteger());
                    if (this->send(Confirmation, senderIP, senderPort))
                        throw std::runtime_error("ERROR - could not send ID Assign packet");
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
        else if (this->isConnectionRequest(packet, senderIP, senderPort))
        {
            sf::Packet Confirmation = this->ConnectionConfirmPacket(senderIP.toInteger());
            if (this->send(Confirmation, senderIP, senderPort))
                throw std::runtime_error("ERROR - could not send ID Assign packet");
            this->onClientConnected.invoke(_threadSafeEvents);
        }
        else if (this->isConnectionClose(packet, senderIP, senderPort))
        {
            this->onClientDisconnected.invoke(_threadSafeEvents);
        }
        else if (this->isPassword(packet, senderIP, senderPort))
        {
            sf::Packet Confirmation = this->ConnectionConfirmPacket(senderIP.toInteger());
            if (this->send(Confirmation, senderIP, senderPort))
                throw std::runtime_error("ERROR - could not send ID Assign packet");
            this->onClientConnected.invoke(_threadSafeEvents);
        }

        packet.clear();
    }
}

// void Server::thread_update(std::stop_token stoken)
// {
//     UpdateLimiter updateLimit(_socketUpdateRate);

//     sf::Clock deltaClock;
//     float deltaTime;
//     sf::Clock secondClock;
//     bool applyPacketsPerSecond = false;
//     while (!stoken.stop_requested())
//     {
//         // TODO do this with an event
//         if (updateLimit.getUpdateLimit() != _socketUpdateRate)
//         {
//             updateLimit.updateLimit(_socketUpdateRate);
//         }

//         deltaTime = deltaClock.restart().asSeconds();
//         _connectionTime += deltaTime;
        
//         applyPacketsPerSecond = false;
//         if (secondClock.getElapsedTime().asSeconds() >= 1)
//         {
//             applyPacketsPerSecond = true;
//             secondClock.restart();
//         }
        
//         for (auto& clientData: _clientData)
//         {
//             clientData.second.TimeSinceLastPacket += deltaTime;
//             if (clientData.second.TimeSinceLastPacket >= _clientTimeoutTime)
//             {
//                 this->disconnectClient(clientData.first);
//                 this->onClientDisconnected.invoke(_threadSafeEvents);
//             }
//             clientData.second.ConnectionTime += deltaTime;

//             if (applyPacketsPerSecond)
//             {
//                 clientData.second.PacketsPerSecond = clientData.second.PacketsSent;
//                 clientData.second.PacketsSent = 0;
//             }
//         }
        
//         if (_sendingPackets) _packetSendFunction.invoke();
        
//         updateLimit.wait();
//     }
// }

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

void Server::initThreadFunctions() 
{
    _secondUpdate.setFunction(&secondUpdate, this);
    _update.setFunction(&update, this);
}

void Server::update(const float& deltaTime) 
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

void Server::secondUpdate() 
{
    for (auto& clientData: _clientData)
    {
        clientData.second.PacketsPerSecond = clientData.second.PacketsSent;
        clientData.second.PacketsSent = 0;
    }
}
