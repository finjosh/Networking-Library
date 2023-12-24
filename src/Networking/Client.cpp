#include "include/Networking/Client.hpp"

Client::Client(IpAddress serverIP, unsigned short serverPort) 
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

void Client::setAndSendPassword(string password)
{ _password = password; this->sendPasswordToServer(); }

void Client::sendPasswordToServer()
{
    Packet temp = this->PasswordPacket(_password);
        if (this->send(temp, _serverIP, _serverPort)) cerr << "could not send password to host" << endl;
}

bool Client::ConnectToServer(funcHelper::func<void> customPacketSendFunction)
{
    Packet connectionRequest = this->ConnectionRequestTemplate();

    if (_serverIP != IpAddress::None)
    {
        if (!this->isReceivingPackets())
        {
            delete(_ssource);
            _ssource = nullptr;
            _ssource = new stop_source;
            this->bind(Socket::AnyPort);
            _receive_thread = new jthread{thread_receive_packets, this, _ssource->get_token()};
        }
        if (_update_thread == nullptr) _update_thread = new jthread{thread_update, this, _ssource->get_token(), customPacketSendFunction};
    }

    if (_serverIP == IpAddress::LocalHost) _id = IpAddress::LocalHost.toInteger();
    else _id = _ip;

    if (this->send(connectionRequest, _serverIP, _serverPort) != Socket::Done)
    {
        StopThreads();
        return false; 
    }

    return true;
}

void Client::setServerData(IpAddress serverIP, unsigned short serverPort)
{
    _serverIP = serverIP; 
    _serverPort = serverPort;  
}

void Client::setServerData(IpAddress serverIP)
{
    _serverIP = serverIP; 
}

void Client::SendToServer(Packet& packet)
{
    if (this->send(packet, _serverIP, _serverPort))
        cerr << "ERROR - could not send packet to the server" << endl;
}

bool Client::isData(Packet& packet)
{
    int temp;

    // making a copy of the packet in case that it is not a connection request
    // if it is a connection request we then remove that int from the packet to make the main script simpler and no need to remember to remove it
    Packet c_packet = packet;
    
    if ((c_packet >> temp) && temp == PacketType::Data)
    {
        packet >> temp; // removing the packet type infomation from the packet
        return true;
    }

    // returning false if this packet is not a data packet
    return false;
}

bool Client::isConnectionClose(Packet packet)
{
    int temp;

    // making a copy of the packet in case that it is not a connection request
    // if it is a connection request we then remove that int from the packet to make the main script simpler and no need to remember to remove it
    Packet c_packet = packet;
    
    if ((c_packet >> temp) && temp == PacketType::ConnectionClose)
    {
        packet >> temp; // removing the packet type infomation from the packet

        packet.clear();

        _id = 0;
        _isOpen_Connected = false;

        return true;
    }

    // returning false if this packet is not a connection close
    return false;
}

bool Client::isConnectionConfirm(Packet& packet)
{
    int temp;

    // making a copy of the packet in case that it is not a connection request
    // if it is a connection request we then remove that int from the packet to make the main script simpler and no need to remember to remove it
    Packet c_packet = packet;
    
    if ((c_packet >> temp) && temp == PacketType::ConnectionConfirm)
    {
        packet >> temp;
        packet >> _id;
        return true;
    }

    // returning false if this packet is not a connection confirm
    return false;
}

bool Client::isWrongPassword(Packet& packet)
{
    int temp;

    // making a copy of the packet in case that it is not a connection request
    // if it is a connection request we then remove that int from the packet to make the main script simpler and no need to remember to remove it
    Packet c_packet = packet;
    
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

bool Client::isPasswordRequest(Packet& packet)
{
    int temp;

    // making a copy of the packet in case that it is not a connection request
    // if it is a connection request we then remove that int from the packet to make the main script simpler and no need to remember to remove it
    Packet c_packet = packet;
    
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
    Packet packet;
    IpAddress senderIP;
    unsigned short senderPort;

    while (!stoken.stop_requested()) {
        Status receiveStatus = this->receive(packet, senderIP, senderPort);
        if (receiveStatus == sf::Socket::Error)
        {
            if (stoken.stop_requested()) break;
            cerr << "ERROR - receiving packet" << endl;
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
            this->DataPackets.push_back(DataPacket(packet));
            _timeSinceLastPacket = 0.0;
            this->onDataReceived.invoke(_threadSafeEvents);
        }
        else if (this->isConnectionClose(packet))
        {              
            _isOpen_Connected = false;
            _id = 0;
            _port = 0;
            this->onConnectionClosed.invoke(_threadSafeEvents);
        }
        else if (this->isConnectionConfirm(packet))
        {
            _isOpen_Connected = true;
            _port = this->getLocalPort();
            this->onConnectionConfirmed.invoke(_threadSafeEvents);
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
        if (this->isConnected()) 
        {
            _open_Connected_Time += deltaTime;
            _timeSinceLastPacket += deltaTime;
        }
        if (_timeSinceLastPacket >= _clientTimeoutTime) { this->Disconnect(); this->onConnectionClosed.invoke(_threadSafeEvents); }
        if (_sendingPackets) customPacketSendFunction();
        updateLimit.wait();
    }
}

void Client::StopThreads()
{
    if (_update_thread != nullptr)
    {
        _ssource->request_stop();
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
    if (this->isConnected())
    {
        Packet close = this->ConnectionCloseTemplate();
        this->SendToServer(close);
    }
    
    if (_isOpen_Connected) 
        this->onConnectionClosed.invoke(_threadSafeEvents);
 
    _isOpen_Connected = false;
    this->_needsPassword = false;
    _wrongPassword = false;
    _password = "";
    _open_Connected_Time = 0.f;
    _timeSinceLastPacket = 0.f;
    _serverIP = IpAddress::None;

    StopThreads();
}

float Client::getTimeSinceLastPacket()
{ return _timeSinceLastPacket; }

IpAddress Client::getServerIP()
{ return _serverIP; }

unsigned int Client::getServerPort()
{ return _serverPort; }
