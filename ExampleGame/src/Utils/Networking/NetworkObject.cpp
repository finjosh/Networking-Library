#include "Utils/Networking/NetworkObject.hpp"
#include "Utils/Networking/NetworkObjectManager.hpp"

bool _networkObjectComp::operator() (const NetworkObject* lhs, const NetworkObject* rhs) const
{
    return lhs->getNetworkID() < rhs->getNetworkID();
}

unsigned long long NetworkObject::_nextID = 1;

NetworkObject::NetworkObject(const size_t& objectType) : _typeid(objectType) {}

NetworkObject::~NetworkObject()
{
    if (_id == 0) // only required in the deconstructor
        return;
    removeNetworkObject();
}

unsigned long long NetworkObject::getNetworkID() const
{
    return _id;
}

size_t NetworkObject::getTypeid() const
{
    return _typeid;
}

void NetworkObject::createNetworkObject()
{
    _id = _nextID++;
    NetworkObjectManager::createNetworkObject(this);
}

void NetworkObject::removeNetworkObject()
{
    NetworkObjectManager::removeNetworkObject(this);
    _id = 0;
}

void NetworkObject::setID(unsigned long long id)
{
    _id = id;
}
