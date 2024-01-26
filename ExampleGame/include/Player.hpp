#ifndef PLAYER_H
#define PLAYER_H

#pragma once

#include "box2d/Box2D.h"

#include "SFML/Graphics.hpp"

#include "Utils/Physics/WorldHandler.hpp"
#include "Utils/Graphics/WindowHandler.hpp"
#include "Utils/UpdateInterface.hpp"

class Player : public sf::RectangleShape, private UpdateInterface
{
public:

    Player(float x, float y);

    void draw(sf::RenderStates rs = sf::RenderStates::Default);
    virtual void Update(float deltaTime) override;

protected:


private:
    b2Body* _body;
};

#endif
