#include "Utils/Physics/WorldHandler.hpp"

b2World WorldHandler::_world({0.f,0.f});

b2World& WorldHandler::getWorld()
{
    return _world;
}
