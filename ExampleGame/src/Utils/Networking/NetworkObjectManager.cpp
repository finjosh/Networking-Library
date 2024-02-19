#include "Utils/Networking/NetworkObjectManager.hpp"

udp::Client* NetworkObjectManager::_client = nullptr;
udp::Server* NetworkObjectManager::_server = nullptr;
bool NetworkObjectManager::_isServer = false;
funcHelper::func<sf::Packet> NetworkObjectManager::_sendPacketFunc;

std::mutex NetworkObjectManager::_threadLock;

EventHelper::Event NetworkObjectManager::onConnectionOpen;
EventHelper::Event NetworkObjectManager::onConnectionClose;
EventHelper::EventDynamic<sf::Packet*> NetworkObjectManager::onDataReceived;

std::unordered_set<NetworkObject*> NetworkObjectManager::_objects;

NetworkObjectManager::_networkObject::_networkObject(unsigned long long id) : 
    NetworkObject(typeid(NetworkObjectManager::_networkObject).hash_code()) 
{
    NetworkObject::setID(id);
}

void NetworkObjectManager::_networkObject::setID(unsigned long long id)
{
    NetworkObject::setID(id);
}

void NetworkObjectManager::init(udp::Server* server, udp::Client* client, funcHelper::func<sf::Packet> sendFunction)
{
    _server = server;
    _client = client;
    _sendPacketFunc.setFunction(sendFunction);

    _client->setThreadSafeOverride(true);

    // _server->onConnectionOpen([](){ onConnectionOpen.invoke(); });
    // _server->onConnectionOpen([](){ _isServer = true; });
    // _server->onConnectionClose([](){ onConnectionClose.invoke(); });
    // _server->onConnectionClose([](){ _isServer = false; });
    // _server->onDataReceived(NetworkObjectManager::_handleData);
    // _server->setPacketSendFunction(_sendData);
    // _client->onConnectionOpen([](){ onConnectionOpen.invoke(); });
    // _client->onConnectionOpen([](){ _isServer = false; });
    // _client->onConnectionClose([](){ onConnectionClose.invoke(); });
    // _client->onDataReceived(NetworkObjectManager::_handleData);
    // _client->setPacketSendFunction(_sendData);
}

udp::Server* NetworkObjectManager::getServer()
{
    return _server;
}

udp::Client* NetworkObjectManager::getClient()
{
    return _client;
}

bool NetworkObjectManager::isServer()
{
    return _isServer;
}

NetworkObject* NetworkObjectManager::getObject(const unsigned long long& networkID)
{
    _networkObject temp(networkID);
    auto obj = _objects.find(&temp);
    if (obj == _objects.end())
        return nullptr;
    return *obj;
}

void NetworkObjectManager::_sendData()
{
    _threadLock.lock();
    sf::Packet packet = udp::Socket::DataPacketTemplate();

    //* Adding the extra data packet
    {
        packet << _sendPacketFunc.invoke();
    }

    //* Adding object data
    // packet << _objects.size();
    // for (NetworkObject* obj: _objects)
    // {
    //     sf::Packet temp = obj->OnSendData();
    //     packet << temp;
    //     // packet << obj->getNetworkID() << obj->getTypeid() << temp.getDataSize();
    //     // packet.append(temp.getData(), temp.getDataSize());
    // }

    if (isServer())
        _server->sendToAll(packet);
    else
        _client->sendToServer(packet);
    _threadLock.unlock();
}

void NetworkObjectManager::_handleData(sf::Packet packet)
{
    sf::Packet temp;

    //* Unpacking the extra data
    {
        temp.clear();
        packet >> temp;
        onDataReceived.invoke(&temp);
    }

    //* Unpacking object data
    // size_t objects;
    // packet >> objects;
    // for (int i = 0; i < objects; i++)
    // {
    //     unsigned long long id;
    //     size_t typeID;
    //     packet >> temp;
    //     // size_t dataSize;
    //     // packet >> id >> typeID >> dataSize;
    //     // temp.append(packet.getData(), dataSize); // TODO does not remove the data from the packet
        
    //     NetworkObject* obj = NetworkObjectManager::getObject(id);
        
    //     if (obj == nullptr)
    //     {
    //         // TODO create the object
    //         continue;
    //     }
    //     obj->OnDataReceived(temp);
    //     temp.clear();
    // }

    // TODO implement the object data handling
}

void NetworkObjectManager::removeNetworkObject(NetworkObject* obj)
{
    _objects.erase(obj);
}

void NetworkObjectManager::createNetworkObject(NetworkObject* obj)
{
    _objects.insert(obj);
}

void NetworkObjectManager::lock()
{
    _threadLock.lock();
}

void NetworkObjectManager::unlock()
{
    _threadLock.unlock();
}
