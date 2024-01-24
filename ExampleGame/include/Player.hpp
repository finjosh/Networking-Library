#ifndef PLAYER_H
#define PLAYER_H

#pragma once

#include "box2d/Box2D.h"

#include "SFML/Graphics.hpp"

#include "Utils/Physics/WorldHandler.hpp"
#include "Utils/Graphics/WindowHandler.hpp"

class Player : public sf::RectangleShape
{
public:

    Player(float x, float y);

    void draw(sf::RenderStates rs = sf::RenderStates::Default);
    void updateInput();

protected:


private:
    b2Body* _body;
};

#endif
