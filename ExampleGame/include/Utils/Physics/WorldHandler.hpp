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
    /// @param interval the interval in which box2d will step, default (1/60)
    static void setUpdateInterval(const float& interval = 1.f/60.f);
    static float getUpdateInterval();
    /// @note only call this before using any physics
    static void setVelocityIterations(const int32& iterations = 8);
    static int32 getVelocityIterations();
    /// @note only call this before using any physics
    static void setPositionIterations(const int32& iterations = 3);
    static int32 getPositionIterations();
    /// @brief The max steps per frame,
    static void setMaxUpdates(const int32& maxUpdates = 10);
    static int32 getMaxUpdates();

private:
    inline WorldHandler() = default;
    
    static b2World _world;
    static double _accumulate;
    static float _updateInterval;
    static int32 _velocityIterations;
    static int32 _positionIterations; 
    static int32 _maxUpdates;
};

#endif
