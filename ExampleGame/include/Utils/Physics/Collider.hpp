#ifndef COLLIDER_H
#define COLLIDER_H

#pragma once

#include "box2d/box2d.h"

#include "Utils/Physics/WorldHandler.hpp"
#include "Utils/Object.hpp"

class Collider : public virtual Object
{
public:
    ~Collider();

protected:
    /// @brief creates a body in the world with the default body def parameters
    void initCollider();
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
    b2Body* _body;
};

#endif
