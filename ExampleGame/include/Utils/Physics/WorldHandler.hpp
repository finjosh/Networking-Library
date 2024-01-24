#ifndef WORLDHANDLER_H
#define WORLDHANDLER_H

#pragma once

#include "Box2D/Box2D.h"

class WorldHandler
{
public:
    static b2World& getWorld();

private:
    inline WorldHandler() = default;
    
    static b2World _world;
};

#endif
