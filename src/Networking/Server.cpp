#include "include/Networking/Server.hpp"

Server::Server(unsigned short port, bool passwordRequired)
{
    _port = port;
}

Server::~Server()
{}

void Server::RequirePassword(bool requirePassword)
{ this->_needsPassword = requirePassword; if (!this->_needsPassword) _password = ""; }

void Server::RequirePassword(bool requirePassword, std::string password)
{ this->_needsPassword = requirePassword; _password = password; }

bool Server::openServer(funcHelper::func<void> customPacketSendFunction)
{
    if (this->bind(_port))
        return false;

    _connectionOpen = true;
    this->StartThreads(customPacketSendFunction);
    this->onServerOpened.invoke(_threadSafeEvents);
    return true;
}

void Server::CloseServer()
{
    sf::Packet closeConnection = this->ConnectionCloseTemplate();
    this->SendToAll(closeConnection);

    if (_ssource != nullptr) _ssource->request_stop();
    if (_update_thread != nullptr)
    {
        _update_thread->join();
        delete(_update_thread);
        _update_thread = nullptr;
    }
    if (this->isReceivingPackets()) 
    {
        _receive_thread->detach();
        delete(_receive_thread);
        _receive_thread = nullptr;
        this->close();
        delete(_ssource);
        _ssource = nullptr;
    }
    this->close();

    if (_connectionOpen)
        this->onServerClosed.invoke(_threadSafeEvents);

    this->_needsPassword = false;
    _password = "";
    _connectionOpen = false;
    // _lastID = 0;
    this->deletedClientIDs.clear();
    this->newClientIDs.clear();
    this->DataPackets.clear();
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
            std::cerr << "ERROR - Server.h (Could not send packet to all client data)" << std::endl;
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
            std::cerr << "ERROR - Could not send packet to client" << std::endl;
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
                std::cerr << "ERROR - could not send password request" << std::endl;
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
                std::cerr << "ERROR - could not send request for password" << std::endl;
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

// ID Server::getLastConnectedClientID()
// { return _lastID; }

bool Server::disconnectClient(ID id)
{
    if (_clientData.find(id) != _clientData.end())
    {
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
            std::cerr << "ERROR - receiving packet" << std::endl;
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
                this->DataPackets.push_back(DataPacket(packet));
                client->second.TimeSinceLastPacket = 0.0;
                client->second.PacketsSent++;
                this->onDataReceived.invoke(_threadSafeEvents);
            }
            // if the sender is not a current client add them
            else
            {   
                if (!this->_needsPassword)
                {
                    // _lastID = senderIP.toInteger();
                    // newClientIDs.push_back(_lastID);
                    _clientData.insert({(ID)(senderIP.toInteger()), ClientData(senderPort, (ID)_ip)});

                    this->DataPackets.push_back(DataPacket(packet));
                    sf::Packet Confirmation = this->ConnectionConfirmPacket(senderIP.toInteger());
                    if (this->send(Confirmation, senderIP, senderPort))
                        std::cerr << "ERROR - could not send ID Assign packet" << std::endl;
                    this->onClientConnected.invoke(_threadSafeEvents);
                }
                else
                {
                    sf::Packet needPassword = this->PasswordRequestPacket();
                    if (this->send(needPassword, senderIP, senderPort) != Socket::Done) 
                        std::cerr << "ERROR - could not send password request" << std::endl;
                }
            }
        }
        else if (this->isConnectionRequest(packet, senderIP, senderPort))
        {
            sf::Packet Confirmation = this->ConnectionConfirmPacket(senderIP.toInteger());
            if (this->send(Confirmation, senderIP, senderPort))
                std::cerr << "ERROR - could not send ID Assign packet" << std::endl;
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
                std::cerr << "ERROR - could not send ID Assign packet" << std::endl;
            this->onClientConnected.invoke(_threadSafeEvents);
        }

        packet.clear();
    }
}

void Server::thread_update(std::stop_token stoken, funcHelper::func<void> customPacketSendFunction)
{
    UpdateLimiter updateLimit(_socketUpdateRate);

    sf::Clock deltaClock;
    float deltaTime;
    sf::Clock secondClock;
    bool applyPacketsPerSecond = false;
    while (!stoken.stop_requested())
    {
        //* TODO make this more efficient
        if (updateLimit.getUpdateLimit() != _socketUpdateRate)
        {
            updateLimit.updateLimit(_socketUpdateRate);
        }
        //* -----------------------------

        deltaTime = deltaClock.restart().asSeconds();
        _connectionTime += deltaTime;
        
        applyPacketsPerSecond = false;
        if (secondClock.getElapsedTime().asSeconds() >= 1)
        {
            applyPacketsPerSecond = true;
            secondClock.restart();
        }
        
        for (auto& clientData: _clientData)
        {
            clientData.second.TimeSinceLastPacket += deltaTime;
            if (clientData.second.TimeSinceLastPacket >= _clientTimeoutTime)
            {
                this->disconnectClient(clientData.first);
                this->onClientDisconnected.invoke(_threadSafeEvents);
            }
            clientData.second.ConnectionTime += deltaTime;

            if (applyPacketsPerSecond)
            {
                clientData.second.PacketsPerSecond = clientData.second.PacketsSent;
                clientData.second.PacketsSent = 0;
            }
        }
        
        if (_sendingPackets) customPacketSendFunction();
        
        updateLimit.wait();
    }
}

void Server::StartThreads(funcHelper::func<void> customPacketSendFunction)
{
    if (!this->isReceivingPackets())
    {
        _ssource = new std::stop_source;
        _receive_thread = new std::jthread{Server::thread_receive_packets, this, _ssource->get_token()};
    }
    if (_update_thread == nullptr) _update_thread = new std::jthread{Server::thread_update, this, _ssource->get_token(), customPacketSendFunction};
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
