#ifndef NETWORKOBJECTMANAGER_H
#define NETWORKOBJECTMANAGER_H

#pragma once

#include <set>

#include "Utils/Networking/NetworkObject.hpp"
#include "Networking/SocketUI.hpp"
#include "Networking/Server.hpp"
#include "Networking/Client.hpp"

#include "Utils/EventHelper.hpp"
#include "Utils/funcHelper.hpp"

class NetworkObject;

class NetworkObjectManager
{
public:
    /// @brief initializes the manager so you can make custom UI for handling server and client connection states
    /// @param sendFunction should return a pointer to a packet (can be allocated on the heap) this should only be extra data not related to any specific object data
    /// @note make sure that the sendFunction is thread safe
    static void init(udp::Server* server, udp::Client* client, funcHelper::func<sf::Packet*> sendFunction);
    static udp::Server* getServer();
    static udp::Client* getClient();

    /// @returns true if the current socket type is server
    static bool isServer();

    static EventHelper::Event onConnectionOpen;
    static EventHelper::Event onConnectionClose;

    /// @brief optional parameter of the packet data
    /// @note This will be called BEFORE any object data is handled
    /// @note this is intended for extra data (other than object data)
    static EventHelper::EventDynamic<sf::Packet*> onDataReceived;

protected:
    /// @brief called by the server/client when sending data
    static void _sendData();
    /// @brief handles the data received 
    static void _handleData(sf::Packet packet);

    static void addNetworkObject(NetworkObject* obj);
    static void removeNetworkObject(NetworkObject* obj);

    friend NetworkObject;

private:
    inline NetworkObjectManager() = default;

    static udp::Client* _client;
    static udp::Server* _server;
    static bool _isServer;
    static funcHelper::func<sf::Packet*> _sendPacketFunc;

    static std::set<NetworkObject*, _networkingObjectComp> _objects;
};

#endif
