#include "Utils/Physics/WorldHandler.hpp"
#include "Utils/Debug/CommandPrompt.hpp"

b2World WorldHandler::_world({0.f,0.f});
double WorldHandler::_accumulate = 0;
float WorldHandler::_updateInterval = 1.f/50.f;
int32 WorldHandler::_velocityIterations = 8;
int32 WorldHandler::_positionIterations = 3;
int32 WorldHandler::_maxUpdates = 10;

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
    Command::Prompt::print("Accumulate: " + std::to_string(_accumulate));
    int32 updates = std::min(int(std::round(_accumulate/_updateInterval)), _maxUpdates);
    Command::Prompt::print("Updates: " + std::to_string(updates));
    _accumulate -= updates*_updateInterval;
    Command::Prompt::print("Left over accumulate: " + std::to_string(_accumulate));
    while (updates > 0)
    {
        _world.Step(_updateInterval, int32(8), int32(3));
        updates--;
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

void WorldHandler::setVelocityIterations(const int32& iterations)
{
    _velocityIterations = iterations;
}

int32 WorldHandler::getVelocityIterations()
{
    return _velocityIterations;
}

void WorldHandler::setPositionIterations(const int32& iterations)
{
    _positionIterations = iterations;
}

int32 WorldHandler::getPositionIterations()
{
    return _positionIterations;
}
