#include "Utils/Physics/WorldHandler.hpp"

b2World WorldHandler::_world;

b2World& WorldHandler::getWorld()
{
    return _world;
}
