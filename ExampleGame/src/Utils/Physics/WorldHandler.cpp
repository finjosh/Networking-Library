#include "Utils/Physics/WorldHandler.hpp"

b2World WorldHandler::_world({0.f,0.f});
double WorldHandler::_accumulate = 0;
float WorldHandler::_updateInterval = 1.f/60.f;

b2World& WorldHandler::getWorld()
{
    return _world;
}

void WorldHandler::updateTime(const double& deltaTime)
{
    _accumulate += deltaTime;
}

void WorldHandler::updateWorld()
{
    while (_accumulate >= _updateInterval)
    {
        _world.Step(_updateInterval, int32(8), int32(3));
        _accumulate -= _updateInterval;
    }
}

void WorldHandler::setUpdateInterval(const float& interval)
{
    _updateInterval = interval;
}

float WorldHandler::getUpdateInterval()
{
    return _updateInterval;
}
