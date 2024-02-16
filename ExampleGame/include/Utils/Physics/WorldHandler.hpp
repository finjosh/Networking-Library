#ifndef WORLDHANDLER_H
#define WORLDHANDLER_H

#pragma once

#include "box2d/Box2D.h"

const float PIXELS_PER_METER = 10;

class WorldHandler
{
public:
    static b2World& getWorld();
    static void updateTime(const double& deltaTime);
    static void updateWorld();
    /// @note only call this before using any physics
    /// @param interval the interval in which box2d will step default (1/60)
    static void setUpdateInterval(const float& interval);
    static float getUpdateInterval();

private:
    inline WorldHandler() = default;
    
    static b2World _world;
    static double _accumulate;
    static float _updateInterval;
};

#endif
