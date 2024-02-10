#include "Utils/Networking/NetworkObject.hpp"

NetworkObject::~NetworkObject()
{
    removeNetworkObject();
}

unsigned long long NetworkObject::getNetworkID() const
{
    return _id;
}

void NetworkObject::initNetworkObject(unsigned long long id)
{
    _id = id;
    NetworkObjectManager::addNetworkObject(this);
}

void NetworkObject::removeNetworkObject()
{
    NetworkObjectManager::removeNetworkObject(this);
}
