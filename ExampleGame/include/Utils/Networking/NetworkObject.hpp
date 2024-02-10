#ifndef NETWORKOBJECT_H
#define NETWORKOBJECT_H

#pragma once

#include "SFML/Network/Packet.hpp"

#include "Utils/Object.hpp"
#include "Utils/Networking/NetworkObjectManager.hpp"

class NetworkObject : public virtual Object
{
public:
    ~NetworkObject();

    unsigned long long getNetworkID() const;
    void initNetworkObject(unsigned long long id);
    /// @brief removes this object from the network
    /// @note does not destroy it
    void removeNetworkObject();

protected:
    /// @brief called when the network connection is opened
    /// @note this is called when either the server is opened or the client is connected
    inline virtual void OnConnectionOpen(){};
    /// @brief called when the network connection is closed
    /// @note this is called when either the server is closed or the client is disconnected 
    inline virtual void OnConnectionClose(){};
    /// @brief called when data is received for this Object
    /// @param data the packet containing data for this obj
    virtual void OnDataReceived(sf::Packet& data) = 0;
    /// @brief Called when data is required from this object
    /// @note make sure that the send function is thread safe
    /// @returns a packet with the data that will be unpacked in "OnDataReceived"
    virtual sf::Packet& OnSendData() = 0;

private:
    /// @brief null id = 0
    unsigned long long _id = 0;
};

class _networkingObjectComp
{
public:
    bool operator() (const NetworkObject* lhs, const NetworkObject* rhs) const;
};

#endif
