#ifndef OBJECTTYPE_H
#define OBJECTTYPE_H

#pragma once

#include <unordered_map>

#include "Utils/funcHelper.hpp"
#include "Utils/Networking/NetworkObject.hpp"

class NetworkObject;

//! Make sure to add a constructor for any network object you make 
static std::unordered_map<size_t, funcHelper::func<NetworkObject*>> __ObjectTypeConstructors;

/// @param objectType should be the typeid(type).hash_code
static void initObjectTypeConstructor(const size_t& objectType, const funcHelper::func<NetworkObject*>& constructor)
{
    __ObjectTypeConstructors.insert({objectType, constructor});
}

#endif
