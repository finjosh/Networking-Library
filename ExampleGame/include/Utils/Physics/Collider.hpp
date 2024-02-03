#ifndef COLLIDER_H
#define COLLIDER_H

#pragma once

#include "box2d/box2d.h"

#include "Utils/Physics/WorldHandler.hpp"
#include "Utils/Object.hpp"

class Collider : public virtual Object
{
public:
    Collider();
    ~Collider();

    /// @brief sets only physics enabled or not ignores the object state
    /// @note if disabled object state does not matter if enabled physics will follow object state
    void setPhysicsEnabled(const bool& enabled = true);
    /// @returns true if physics are enabled
    bool isPhysicsEnabled() const;

protected:
    /// @brief creates a body in the world with the default body def parameters
    void initCollider(const float& x = 0, const float& y = 0);
    /// @brief create a body in the world with the default body def parameters
    /// @param pos the position to init the body at
    void initCollider(const b2Vec2& pos);
    /// @brief create a body in the world with the given body def
    void initCollider(const b2BodyDef& bodyDef);
    // /// @brief creates a fixture with the given shape and default parameters for the fixture
    // /// @returns the pointer to the new fixture
    // b2Fixture* createFixture(const b2Shape& shape);
    /// @brief creates a fixture with the given fixture def
    /// @returns the pointer to the new fixture
    b2Fixture* createFixture(const b2FixtureDef& fixture);
    /// @brief creates a fixture with the given shape and density
    /// @returns the pointer to the new fixture
    b2Fixture* createFixture(const b2Shape& shape, const float& density);
    // TODO make a function that takes in all vars for a fixture and the shape

    /// @warning DO NOT DESTROY THE BODY
    /// @note if you want to destroy the physics body call destroy on this object
    b2Body* operator-> () const;
    /// @warning DO NOT DESTROY THE BODY
    /// @note if you want to destroy the physics body call destroy on this object
    b2Body* operator* () const;
    /// @warning DO NOT DESTROY THE BODY
    /// @note if you want to destroy the physics body call destroy on this object
    b2Body* getBody() const;

private:
    /// @brief updates the body state (enabled or not)
    void updatePhysicsState();

    b2Body* _body;
    /// @brief if true follows object else physics are disabled no matter object state
    bool _enabled = true;
};

#endif
