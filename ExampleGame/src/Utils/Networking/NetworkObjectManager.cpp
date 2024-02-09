#include "Utils/Networking/NetworkObjectManager.hpp"

udp::Client* NetworkObjectManager::_client = nullptr;
udp::Server* NetworkObjectManager::_server = nullptr;
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
    _server->onConnectionClose(EventHelper::Event::invoke, onConnectionClose);
    _server->onDataReceived(NetworkObjectManager::_handleData);
    _client->onConnectionOpen(EventHelper::Event::invoke, onConnectionOpen);
    _client->onConnectionClose(EventHelper::Event::invoke, onConnectionClose);
    _client->onDataReceived(NetworkObjectManager::_handleData);
}

udp::Server* NetworkObjectManager::getServer()
{
    return _server;
}

udp::Client* NetworkObjectManager::getClient()
{
    return _client;
}

void NetworkObjectManager::_sendData()
{
    // TODO implement this
}

void _handleData()
{
    // TODO implement this
}