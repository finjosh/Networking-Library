#ifndef NETWORKOBJECT_H
#define NETWORKOBJECT_H

#pragma once

#include "SFML/Network/Packet.hpp"

#include "Utils/Object.hpp"

/*
TODO Make an object type that is given to a network object at creation
the object manager should have a list of constructors for each type 
the objects should be able to be made on one part of the network and automatically update on the other parts
*/
/// @warning when and object is used on a network make sure to only use fixed size types
/// @note fixed sized types are built in with SFML
class NetworkObject : public virtual Object
{
public:
    /// @param objectType the type of object that is being made
    /// @note the objectType should be declared in the ObjectType.hpp file first
    NetworkObject(const size_t& objectType);
    ~NetworkObject();

    unsigned long long getNetworkID() const;
    size_t getTypeid() const;
    void createNetworkObject();
    /// @brief removes this object from the network
    /// @note does not destroy this object
    void removeNetworkObject();

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
    virtual sf::Packet OnSendData() = 0;

protected:
    /// @warning only use this if you know what you are doing
    void setID(unsigned long long id);

private:
    /// @brief null id = 0
    unsigned long long _id = 0;
    const size_t _typeid = 0;

    static unsigned long long _nextID;
};

class _networkObjectComp
{
public:
    bool operator() (const NetworkObject* lhs, const NetworkObject* rhs) const;
};

#endif
