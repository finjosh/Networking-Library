#include "Utils/Networking/NetworkObject.hpp"

std::atomic_ullong NetworkObject::_lastID = 1;

unsigned long int NetworkObject::getNetworkID() const
{
    return _id;
}
