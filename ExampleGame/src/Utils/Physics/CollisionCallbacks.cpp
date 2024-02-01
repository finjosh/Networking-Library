#include "Utils/Physics/CollisionCallbacks.hpp"

void CollisionCallbacks::setBody(b2Body* body)
{
    body->GetUserData().pointer = (uintptr_t)this;
}
