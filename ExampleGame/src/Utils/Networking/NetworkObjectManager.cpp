#include "Utils/Networking/NetworkObjectManager.hpp"

udp::Client* NetworkObjectManager::_client = nullptr;
udp::Server* NetworkObjectManager::_server = nullptr;
bool NetworkObjectManager::_isServer = false;
funcHelper::func<sf::Packet*> _sendPacketFunc();

EventHelper::Event NetworkObjectManager::onConnectionOpen;
EventHelper::Event NetworkObjectManager::onConnectionClose;
EventHelper::EventDynamic<sf::Packet*> NetworkObjectManager::onDataReceived;

void NetworkObjectManager::init(udp::Server* server, udp::Client* client, funcHelper::func<sf::Packet*> sendFunction)
{
    _server = server;
    _client = client;
    _sendPacketFunc.setFunction(sendFunction);

    _server->onConnectionOpen(EventHelper::Event::invoke, onConnectionOpen);
    _server->onConnectionOpen([](){ _isServer = true; });
    _server->onConnectionClose(EventHelper::Event::invoke, onConnectionClose);
    _server->onConnectionClose([](){ _isServer = false; });
    _server->onDataReceived(NetworkObjectManager::_handleData);
    _server->setPacketSendFunction(_sendData);
    _client->onConnectionOpen(EventHelper::Event::invoke, onConnectionOpen);
    _client->onConnectionOpen([](){ _isServer = false; });
    _client->onConnectionClose(EventHelper::Event::invoke, onConnectionClose);
    _client->onDataReceived(NetworkObjectManager::_handleData);
    _client->setPacketSendFunction(_sendData);
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

void NetworkObjectManager::_sendData()
{
    // TODO implement this
    sf::Packet packet;
    packet << (*_sendPacketFunc.invoke());
}

void NetworkObjectManager::_handleData(sf::Packet packet)
{
    // TODO implement this
    sf::Packet temp;

    // unpacking the extra data
    temp << packet;
    onDataReceived.invoke(&temp);

    // TODO implement the object data handling
}

void NetworkObjectManager::addNetworkObject(NetworkObject* obj)
{
    _objects.insert(obj);
}

void NetworkObjectManager::removeNetworkObject(NetworkObject* obj)
{
    _objects.erase(obj);
}
