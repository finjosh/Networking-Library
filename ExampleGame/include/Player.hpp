#ifndef PLAYER_H
#define PLAYER_H

#pragma once

#include <math.h>

#include "box2d/Box2D.h"

#include "SFML/Graphics.hpp"

#include "Utils/Physics/WorldHandler.hpp"
#include "Utils/Graphics/WindowHandler.hpp"
#include "Utils/UpdateInterface.hpp"
#include "Utils/Graphics/Drawable.hpp"

class Player : public sf::RectangleShape, public UpdateInterface, public DrawableObj
{
public:
    Player(const float& x, const float& y, const int& layer = 10);
    ~Player();

protected:
    virtual void Update(float deltaTime) override;
    virtual void Draw(sf::RenderWindow& window) override;

private:
    b2Body* _body;
    float _burstCooldown = 0.f;
};

#endif
