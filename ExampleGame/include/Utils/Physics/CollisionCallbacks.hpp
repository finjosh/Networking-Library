#ifndef COLLISIONCALLBACK_H
#define COLLISIONCALLBACK_H

#pragma once

#include "box2d/b2_world_callbacks.h"
#include "box2d/b2_body.h"

#include "Utils/Object.hpp"

class CollisionCallbacks : public virtual Object
{
public:
    /// @warning CALL THIS WILL THE GIVEN BODY OR ELSE NO CALLBACKS WILL WORK
    void setBody(b2Body* body);

    inline virtual void BeginContact(b2Contact* contact) {};
    inline virtual void EndContact(b2Contact* contact) {};
    inline virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) {};
    inline virtual void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) {};

protected:

private:

};

#endif
