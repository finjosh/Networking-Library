#ifndef NETWORKOBJECT_H
#define NETWORKOBJECT_H

#pragma once

#include <atomic>

#include "SFML/Network/Packet.hpp"

#include "Utils/Object.hpp"

class NetworkObject : public virtual Object
{
public:
    unsigned long int getNetworkID() const;

protected:
    /// @brief called when the network connection is opened
    /// @note this is called when either the server is opened or the client is connected
    inline virtual void OnConnectionOpen(){};
    /// @brief called when the network connection is closed
    /// @note this is called when either the server is closed or the client is disconnected 
    inline virtual void OnConnectionClose(){};
    /// @brief called when data is received for this Object
    /// @param data the data for this obj
    virtual void OnDataReceived(sf::Packet& data) = 0;
    /// @brief Called when data is required from this object
    /// @note make sure that the send function is thread safe
    /// @returns a packet with the data that will be unpacked in "OnDataReceived"
    virtual sf::Packet& OnSendData() = 0;

private:
    /// @brief null id = 0
    /// @note this is the id that is stored on the server
    unsigned long long _id = 0;

    static std::atomic_ullong _lastID;
};

class _networkingObjectComp
{
public:
    bool operator() (const NetworkObject* lhs, const NetworkObject* rhs) const;
};

#endif
