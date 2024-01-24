#ifndef WORLDHANDLER_H
#define WORLDHANDLER_H

#pragma once

#include "box2d/Box2D.h"

const float PIXELS_PER_METER = 10;

class WorldHandler
{
public:
    static b2World& getWorld();

private:
    inline WorldHandler() = default;
    
    static b2World _world;
};

#endif
