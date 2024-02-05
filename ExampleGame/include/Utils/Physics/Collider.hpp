#ifndef COLLIDER_H
#define COLLIDER_H

#pragma once

#include "box2d/box2d.h"

#include "Utils/Physics/WorldHandler.hpp"
#include "Utils/Object.hpp"

#include "Utils/Physics/CollisionManager.hpp"

class CollisionManager;

class Collider;

class CollisionData
{
public:
    CollisionData(Collider* collider, b2Fixture* thisFixture, b2Fixture* otherFixture);

    /// @returns the other objects collider
    Collider* getCollider();
    /// @returns the fixture from this object that collided
    b2Fixture* getFixtureA();
    /// @returns the fixture from the other object that collided
    b2Fixture* getFixtureB();

private:
    Collider* _collider;
    b2Fixture* _thisFixture;
    b2Fixture* _otherFixture;
};

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

    /// @brief called after the time step so the body can be destroyed
    /// @param collisionData the collision data
    inline virtual void BeginContact(CollisionData collisionData) {};
    /// @brief called after the time step so the body can be destroyed
    /// @param collisionData the collision data
    inline virtual void EndContact(CollisionData collisionData) {};
    /// @brief This can be called multiple times in one frame (called before the collision is handled)
    /// @note to get the collider get the userdata from the body and cast to Collider
    /// @warning do not destroy the body during this call back
    /// @param contact the contact data
    /// @param oldManifold the old manifold data
    inline virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) {};
    /// @brief This can be called multiple times in one frame (called after the collision is handled)
    /// @note to get the collider get the userdata from the body and cast to Collider
    /// @warning do not destroy the body during this call back
    /// @param contact the contact data
    /// @param impulse the impulse data
    inline virtual void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) {};

    friend CollisionManager;

private:
    /// @brief updates the body state (enabled or not)
    void updatePhysicsState();

    b2Body* _body;
    /// @brief if true follows object else physics are disabled no matter object state
    bool _enabled = true;
};

#endif
