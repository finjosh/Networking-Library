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

// TODO figure out how objects can be made on the server the the client will be able to update
class NetworkObjectManager
{
public:
    /// @brief initializes the manager so you can make custom UI for handling server and client connection states
    /// @param sendFunction should return a pointer to a packet (can be allocated on the heap) this should only be extra data not related to a specific object
    /// @note make sure that the sendFunction is thread safe
    static void init(udp::Server* server, udp::Client* client, funcHelper::func<sf::Packet*> sendFunction);
    static udp::Server* getServer();
    static udp::Client* getClient();

    static EventHelper::Event onConnectionOpen;
    static EventHelper::Event onConnectionClose;

    /// @brief optional parameter of the packet data
    /// @note this is only the extra data that is sent
    /// @note does NOT contain any object data
    static EventHelper::EventDynamic<sf::Packet*> onDataReceived;

protected:
    /// @brief called by the server/client when sending data
    static void _sendData();
    /// @brief handles the data received 
    static void _handleData();

private:
    inline NetworkObjectManager() = default;

    static udp::Client* _client;
    static udp::Server* _server;
    static funcHelper::func<sf::Packet*> _sendPacketFunc;
};

#endif
