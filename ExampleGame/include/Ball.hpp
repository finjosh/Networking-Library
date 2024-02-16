#ifndef BALL_H
#define BALL_H

#pragma once

#include "Utils/GameObject.hpp"
#include "Utils/Physics/Collider.hpp"
#include "Utils/Networking/NetworkObject.hpp"

#include "Utils/Physics/WorldHandler.hpp"
#include "Utils/Graphics/WindowHandler.hpp"
#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Window.hpp"

class Ball : public GameObject, public sf::CircleShape, public Collider, public NetworkObject
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

    virtual void OnDataReceived(sf::Packet& data) override;
    virtual sf::Packet OnSendData() override;

protected:
    void Update(const float& deltaTime) override;
    void Draw(sf::RenderWindow& window) override;

    void BeginContact(CollisionData collisionData) override;
    void EndContact(CollisionData collisionData) override;

private:

};

#endif
