#include "include/Networking/Client.hpp"

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

Client::~Client()
{}

bool Client::WasIncorrectPassword()
{ bool temp = _wrongPassword; _wrongPassword = false; return temp; }

void Client::setAndSendPassword(std::string password)
{ setPassword(password); this->sendPasswordToServer(); }

void Client::sendPasswordToServer()
{
    sf::Packet temp = this->PasswordPacket(_password);
        if (this->send(temp, _serverIP, _serverPort)) throw std::runtime_error("could not send password to host");
}

bool Client::ConnectToServer(funcHelper::func<void> customPacketSendFunction)
{
    sf::Packet connectionRequest = this->ConnectionRequestTemplate();

    if (_serverIP != sf::IpAddress::None)
    {
        if (!this->isReceivingPackets())
        {
            delete(_sSource);
            _sSource = nullptr;
            _sSource = new std::stop_source;
            this->bind(Socket::AnyPort);
            _receive_thread = new std::jthread{thread_receive_packets, this, _sSource->get_token()};
        }
        if (_update_thread == nullptr) _update_thread = new std::jthread{thread_update, this, _sSource->get_token(), customPacketSendFunction};
    }

    if (_serverIP == sf::IpAddress::LocalHost) _ip = sf::IpAddress::LocalHost.toInteger();
    else _ip = _ip;

    if (this->send(connectionRequest, _serverIP, _serverPort) != Socket::Done)
    {
        StopThreads();
        return false; 
    }

    return true;
}

void Client::setServerData(sf::IpAddress serverIP, unsigned short serverPort)
{
    _serverIP = serverIP; 
    _serverPort = serverPort;  
}

void Client::setServerData(sf::IpAddress serverIP)
{
    _serverIP = serverIP; 
}

void Client::setServerData(Port port)
{
    _serverPort = port;
}

void Client::SendToServer(sf::Packet& packet)
{
    if (this->send(packet, _serverIP, _serverPort))
        throw std::runtime_error("ERROR - could not send packet to the server");
}

bool Client::isData(sf::Packet& packet)
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

bool Client::isConnectionClose(sf::Packet packet)
{
    int temp;

    // making a copy of the packet in case that it is not a connection request
    // if it is a connection request we then remove that int from the packet to make the main script simpler and no need to remember to remove it
    sf::Packet c_packet = packet;
    
    if ((c_packet >> temp) && temp == PacketType::ConnectionClose)
    {
        packet >> temp; // removing the packet type infomation from the packet

        packet.clear();

        _connectionOpen = false;

        return true;
    }

    // returning false if this packet is not a connection close
    return false;
}

bool Client::isConnectionConfirm(sf::Packet& packet)
{
    int temp;

    // making a copy of the packet in case that it is not a connection request
    // if it is a connection request we then remove that int from the packet to make the main script simpler and no need to remember to remove it
    sf::Packet c_packet = packet;
    
    if ((c_packet >> temp) && temp == PacketType::ConnectionConfirm)
    {
        packet >> temp;
        packet >> _ip;
        return true;
    }

    // returning false if this packet is not a connection confirm
    return false;
}

bool Client::isWrongPassword(sf::Packet& packet)
{
    int temp;

    // making a copy of the packet in case that it is not a connection request
    // if it is a connection request we then remove that int from the packet to make the main script simpler and no need to remember to remove it
    sf::Packet c_packet = packet;
    
    if ((c_packet >> temp) && temp == PacketType::WrongPassword)
    {
        packet >> temp;
        _wrongPassword = true;
        return true;
    }

    _wrongPassword = false;
    // returning false if this packet is not a connection confirm
    return false;
}

bool Client::isPasswordRequest(sf::Packet& packet)
{
    int temp;

    // making a copy of the packet in case that it is not a connection request
    // if it is a connection request we then remove that int from the packet to make the main script simpler and no need to remember to remove it
    sf::Packet c_packet = packet;
    
    if ((c_packet >> temp) && temp == PacketType::RequestPassword)
    {
        packet >> temp;
        this->_needsPassword = true;
        return true;
    }

    // returning false if this packet is not a connection confirm
    return false;
}

void Client::thread_receive_packets(std::stop_token stoken)
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
            this->bind(sf::Socket::AnyPort);
            packet.clear();
            // skip over rest of loop to prevent crash
            continue;
        }
        // TODO same as server
        else if (receiveStatus != sf::Socket::Done)
        {
            packet.clear();
            continue;
        }

        if (this->isData(packet))
        {            
            // this->DataPackets.push_back(DataPacket(packet)); // TODO remove this after ensuring that the event works
            _timeSinceLastPacket = 0.0;
            this->onDataReceived.invoke(packet, _threadSafeEvents);
        }
        else if (this->isConnectionClose(packet))
        {              
            _connectionOpen = false;
            _port = 0;
            this->onConnectionClose.invoke(_threadSafeEvents);
        }
        else if (this->isConnectionConfirm(packet))
        {
            _connectionOpen = true;
            _port = this->getLocalPort();
            this->onConnectionOpen.invoke(_threadSafeEvents);
        }
        else if (this->isPasswordRequest(packet))
        {
            this->onPasswordRequest.invoke(_threadSafeEvents);
        }
        else if (this->isWrongPassword(packet))
        {
            this->onWrongPassword.invoke(_threadSafeEvents);
        }

        packet.clear();
    }
}

void Client::thread_update(std::stop_token stoken, funcHelper::func<void> customPacketSendFunction)
{
    UpdateLimiter updateLimit(_socketUpdateRate);
    sf::Clock deltaClock;
    float deltaTime;

    while (!stoken.stop_requested())
    {
        deltaTime = deltaClock.restart().asSeconds();
        if (this->isConnectionOpen()) 
        {
            _connectionTime += deltaTime;
            _timeSinceLastPacket += deltaTime;
        }
        if (_timeSinceLastPacket >= _clientTimeoutTime) { this->Disconnect(); this->onConnectionClose.invoke(_threadSafeEvents); }
        if (_sendingPackets) customPacketSendFunction.invoke();
        updateLimit.wait();
    }
}

void Client::StopThreads()
{
    if (_update_thread != nullptr)
    {
        _sSource->request_stop();
        _update_thread->detach();
        delete(_update_thread);
        _update_thread = nullptr;
    }
    if (this->isReceivingPackets())
    {
        _receive_thread->detach();
        this->close();
        delete(_receive_thread);
        _receive_thread = nullptr;
    }
}

void Client::Disconnect()
{
    if (this->isConnectionOpen())
    {
        sf::Packet close = this->ConnectionCloseTemplate();
        this->SendToServer(close);
    }
    
    if (_connectionOpen) 
        this->onConnectionClose.invoke(_threadSafeEvents);
 
    _connectionOpen = false;
    _needsPassword = false;
    _wrongPassword = false;
    this->setPassword("");
    _connectionTime = 0.f;
    _timeSinceLastPacket = 0.f;
    _serverIP = sf::IpAddress::None;

    StopThreads();
}

float Client::getTimeSinceLastPacket()
{ return _timeSinceLastPacket; }

sf::IpAddress Client::getServerIP()
{ return _serverIP; }

unsigned int Client::getServerPort()
{ return _serverPort; }
