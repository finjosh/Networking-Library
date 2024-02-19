#ifndef NETWORKOBJECTMANAGER_H
#define NETWORKOBJECTMANAGER_H

#pragma once

#include <set>
#include <mutex>

#include "Utils/Networking/NetworkObject.hpp"
#include "Networking/SocketUI.hpp"
#include "Networking/Server.hpp"
#include "Networking/Client.hpp"

#include "Utils/EventHelper.hpp"
#include "Utils/funcHelper.hpp"

class NetworkObjectManager
{
public:
    /// @brief initializes the manager so you can make custom UI for handling server and client connection states
    /// @param sendFunction should return a pointer to a packet (can be allocated on the heap) this should only be extra data not related to any specific object data
    /// @note make sure that the sendFunction is thread safe
    static void init(udp::Server* server, udp::Client* client, funcHelper::func<sf::Packet> sendFunction);
    static udp::Server* getServer();
    static udp::Client* getClient();

    /// @returns true if the current socket type is server
    static bool isServer();

    /// @warning do NOT store this ptr, get the object pointer if storage is required
    /// @returns nullptr if the network object does not exist 
    static NetworkObject* getObject(const unsigned long long& networkID);

    static EventHelper::Event onConnectionOpen;
    static EventHelper::Event onConnectionClose;

    /// @brief optional parameter of the packet data
    /// @note This will be called BEFORE any object data is handled
    /// @note this is intended for extra data (other than object data)
    static EventHelper::EventDynamic<sf::Packet*> onDataReceived;

    /// @brief stops the network manager from collecting data while it may be in use
    /// @warning do NOT forget to unlock after locking
    static void lock();
    /// @brief lets the network manager collect data again
    static void unlock();
    // TODO probably not the best way to do this

protected:
    /// @brief called by the server/client when sending data
    static void _sendData();
    /// @brief handles the data received 
    static void _handleData(sf::Packet packet);

    /// @note not thread safe
    static void removeNetworkObject(NetworkObject* obj);
    /// @note not thread safe
    static void createNetworkObject(NetworkObject* obj);

    friend NetworkObject;

private:
    inline NetworkObjectManager() = default;

    static udp::Client* _client;
    static udp::Server* _server;
    static bool _isServer;
    static funcHelper::func<sf::Packet> _sendPacketFunc;

    static std::unordered_set<NetworkObject*> _objects;

    static std::mutex _threadLock;

    /// @note used as network object can not be made on its own due to its pure virtual destroy function
    class _networkObject : public NetworkObject
    {
    public:
        _networkObject(unsigned long long id);
        void setID(unsigned long long id);
        createDestroy();
        inline void OnDataReceived(sf::Packet& data) {};
        inline sf::Packet OnSendData() { return sf::Packet(); };
    };
};

#endif
