#ifndef PLAYER_H
#define PLAYER_H

#pragma once

#include <math.h>
#include <mutex>

#include "box2d/Box2D.h"

#include "SFML/Graphics.hpp"

#include "Utils/Physics/WorldHandler.hpp"
#include "Utils/Graphics/WindowHandler.hpp"
#include "Utils/Physics/Collider.hpp"
#include "Utils/GameObject.hpp"
#include "Ball.hpp"
#include "Utils/Networking/NetworkObject.hpp"

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

class Player final : public sf::RectangleShape, public GameObject, public Collider, public NetworkObject
{
public:
    Player(const float& x, const float& y, const bool& handleInput = true, const int& layer = 0);
    ~Player();

    virtual void destroy() override;

    void throwBall() const;

    bool getHandleInput() const;
    void setHandleInput(const bool& handle = true);

    virtual void OnDataReceived(sf::Packet& data) override;
    virtual sf::Packet OnSendData() override;

protected:
    virtual void Update(const float& deltaTime) override;
    virtual void Draw(sf::RenderWindow& window) override;
    void handleInput();

    void BeginContact(CollisionData collisionData) override;
    void EndContact(CollisionData collisionData) override;

private:
    PlayerState _state;
    // TODO make a const for the cool downs and change these to timers
    float _burstCooldown = 0.f;
    float _shootCooldown = 0.f;
    bool _handleInput = true;
};

#endif
