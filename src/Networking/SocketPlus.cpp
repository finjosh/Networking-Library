#include "include/Networking/SocketPlus.hpp"

//* initializer and deconstructor

SocketPlus::SocketPlus()
{
    _ip = IpAddress::getPublicAddress().toInteger();
    _port = this->getLocalPort();
}

SocketPlus::~SocketPlus()
{   
    if (_ssource != nullptr) _ssource->request_stop();
    if (_receive_thread != nullptr) 
    {
        _receive_thread->detach();
    }
    if (_update_thread != nullptr)
    {
        _update_thread->detach();
    }
    delete(_receive_thread);
    delete(_update_thread);
    delete(_ssource);
    this->close();
}

// ------------------------------

//* Getter

ID SocketPlus::getID()
{ return _id; }

IpAddress SocketPlus::getIP()
{ return IpAddress(_id); }

IP SocketPlus::getIP_I()
{ return _id; }

double SocketPlus::getConnectionTime()
{ return _open_Connected_Time; }

double SocketPlus::getOpenTime()
{ return _open_Connected_Time; }

unsigned int SocketPlus::getUpdateInterval()
{ return _socketUpdateRate; }

unsigned int SocketPlus::getPort()
{ return _port; }

int SocketPlus::getClientTimeout()
{ return _clientTimeoutTime; }

// -------

//* Setters

void SocketPlus::setUpdateInterval(unsigned int interval)
{ this->_socketUpdateRate = interval; }

void SocketPlus::sendingPackets(bool sendPackets)
{ this->_sendingPackets = sendPackets; }

void SocketPlus::setPassword(string password)
{ this->_password = password; }

void SocketPlus::setClientTimeout(const int& timeout)
{ 
    _clientTimeoutTime = timeout; 
    onUpdateRateChanged.invoke(_threadSafeEvents);
}

// --------

//* Boolean question Functions

bool SocketPlus::isConnectionRequest(Packet packet)
{
    int temp;
    
    if ((packet >> temp) && temp == PacketType::ConnectionClose)
        return true;

    // returning false if this packet is not a connection close
    return false;
}

bool SocketPlus::isConnectionClose(Packet packet)
{
    int temp;
    
    if ((packet >> temp) && temp == PacketType::ConnectionClose)
        return true;


    // returning false if this packet is not a connection close
    return false;
}

bool SocketPlus::isData(Packet packet)
{
    int temp;
    
    if ((packet >> temp) && temp == PacketType::ConnectionClose)
        return true;

    // returning false if this packet is not a connection close
    return false;
}

bool SocketPlus::isConnectionConfirm(Packet packet)
{
    int temp;
    
    if ((packet >> temp) && temp == PacketType::ConnectionConfirm)
        return true;

    // returning false if this packet is not a connection confirm
    return false;
}

bool SocketPlus::isPasswordRequest(Packet packet)
{
    int temp;
    
    if ((packet >> temp) && temp == PacketType::RequestPassword)
        return true;

    // returning false if this packet is not a connection confirm
    return false;
}

bool SocketPlus::isPassword(Packet packet)
{
    int temp;
    
    if ((packet >> temp) && temp == PacketType::Password)
        return true;

    // returning false if this packet is not a connection confirm
    return false;
}

bool SocketPlus::isWrongPassword(Packet packet)
{
    int temp;
    
    if ((packet >> temp) && temp == PacketType::WrongPassword)
        return true;
    // returning false if this packet is not a connection confirm
    return false;
}

bool SocketPlus::isOpen()
{ return _isOpen_Connected; }

bool SocketPlus::isConnected()
{ return _isOpen_Connected; }

bool SocketPlus::isReceivingPackets()
{
    return (_receive_thread != nullptr);
}

bool SocketPlus::isSendingPackets()
{ return _sendingPackets; }

bool SocketPlus::NeedsPassword()
{ return this->_needsPassword; }

bool SocketPlus::isValidIpAddress(IpAddress ipAddress)
{
    if (ipAddress != IpAddress::None) return true;
    else return false;
}

bool SocketPlus::isValidIpAddress(Uint32 ipAddress)
{
    if (IpAddress(ipAddress) != IpAddress::None) return true;
    else return false;
}

bool SocketPlus::isValidIpAddress(string ipAddress)
{
    if (IpAddress(ipAddress) != IpAddress::None) return true;
    else return false;
}

bool SocketPlus::isThreadSafeEvents()
{
    return _threadSafeEvents;
}

// ---------------------------

//* Other Useful functions

void SocketPlus::ClearDataPackets()
{ this->DataPackets.clear(); }

void SocketPlus::ClearEmptyPackets()
{
    for (list<DataPacket>::iterator temp = this->DataPackets.begin(); temp != this->DataPackets.end(); ++temp)
    {
        if (temp->packet.endOfPacket())
            this->DataPackets.erase(temp); 
    }
}

// ------------------------

//* Template Functions

Packet SocketPlus::ConnectionCloseTemplate()
{
    Packet out;
    out << PacketType::ConnectionClose;
    return out;
}

Packet SocketPlus::ConnectionRequestTemplate()
{
    Packet out;
    out << PacketType::ConnectionRequest;
    return out;
}

Packet SocketPlus::DataPacketTemplate()
{
    Packet out;
    out << PacketType::Data;
    return out;
}

Packet SocketPlus::ConnectionConfirmPacket(Uint32 id)
{
    Packet out;
    out << PacketType::ConnectionConfirm;
    out << id;
    return out;
}

Packet SocketPlus::PasswordRequestPacket()
{
    Packet out;
    out << PacketType::RequestPassword;
    return out;
}

Packet SocketPlus::PasswordPacket(string password)
{
    Packet out;
    out << PacketType::Password;
    out << password;
    return out;
}

Packet SocketPlus::WrongPasswordPacket()
{
    Packet out;
    out << PacketType::WrongPassword;
    return out;
}

// -------------------
