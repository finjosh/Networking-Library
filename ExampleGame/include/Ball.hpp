#ifndef BALL_H
#define BALL_H

#pragma once

#include "Utils/GameObject.hpp"
#include "Utils/Physics/Collider.hpp"

#include "Utils/Physics/WorldHandler.hpp"
#include "Utils/Graphics/WindowHandler.hpp"
#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Window.hpp"

//! TESTING
#include "Utils/Physics/CollisionCallbacks.hpp"

class Ball : public GameObject, public sf::CircleShape, public CollisionCallbacks, public Collider
{
public:
    /// @brief creates a new ball and sets the speed
    /// @note dont store this ball
    /// @param pos position in terms of box2d
    /// @param direction direction of throw
    /// @param speed speed of throw
    Ball(const b2Vec2& pos, const b2Vec2& direction, const float& speed);
    ~Ball();

    createDestroy();

protected:
    void Update(const float& deltaTime) override;
    void Draw(sf::RenderWindow& window) override;

    void BeginContact(b2Contact* contact) override;
    void EndContact(b2Contact* contact) override;

private:

};

#endif
