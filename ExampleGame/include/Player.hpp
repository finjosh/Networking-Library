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

class Player : public sf::RectangleShape, public GameObject
{
public:
    Player(const float& x, const float& y, const int& layer = 0);
    ~Player();

    virtual void destroy() override;

    void throwBall() const;

protected:
    virtual void Update(const float& deltaTime) override;
    virtual void Draw(sf::RenderWindow& window) override;

private:
    b2Body* _body;
    // TODO make a const for the cool downs and change these to timers
    float _burstCooldown = 0.f;
    float _shootCooldown = 0.f;
};

#endif
