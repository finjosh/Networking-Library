#ifndef PLAYER_H
#define PLAYER_H

#pragma once

#include <math.h>

#include "box2d/Box2D.h"

#include "SFML/Graphics.hpp"

#include "Utils/Physics/WorldHandler.hpp"
#include "Utils/Graphics/WindowHandler.hpp"
#include "Utils/GameObject.hpp"
#include "Ball.hpp"

//! TESTING
#include "Utils/Physics/CollisionCallbacks.hpp"

struct PlayerState
{
    bool up = false;
    bool down = false;
    bool right = false;
    bool left = false;
    bool burst = false;
    bool shoot = false;

    inline void reset()
    {
        up = false;
        down = false;
        right = false;
        left = false;
        burst = false;
        shoot = false;
    }
};

class Player : public sf::RectangleShape, public GameObject, public CollisionCallbacks
{
public:
    Player(const float& x, const float& y, const bool& handleInput = true, const int& layer = 0);
    ~Player();

    virtual void destroy() override;

    void throwBall() const;

protected:
    virtual void Update(const float& deltaTime) override;
    virtual void Draw(sf::RenderWindow& window) override;
    void handleInput();

    void BeginContact(b2Contact* contact) override;
    void EndContact(b2Contact* contact) override;

private:
    b2Body* _body;
    PlayerState _state;
    // TODO make a const for the cool downs and change these to timers
    float _burstCooldown = 0.f;
    float _shootCooldown = 0.f;
    bool _handleInput = true;
};

#endif
