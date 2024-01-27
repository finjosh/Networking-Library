#include "Player.hpp"

Player::Player(const float& x, const float& y, const int& layer) : DrawableObj(layer)
{
    b2BodyDef bodyDef;
    bodyDef.position.Set(x/PIXELS_PER_METER, y/PIXELS_PER_METER);
    bodyDef.type = b2_dynamicBody;
    bodyDef.linearDamping = 1;
    b2PolygonShape b2shape;
    b2shape.SetAsBox(10/PIXELS_PER_METER/2.0, 10/PIXELS_PER_METER/2.0);
    b2FixtureDef fixtureDef;
    fixtureDef.density = 1.0;
    fixtureDef.friction = 0.1;
    fixtureDef.restitution = 0.1;
    fixtureDef.shape = &b2shape;

    _body = WorldHandler::getWorld().CreateBody(&bodyDef);
    _body->CreateFixture(&fixtureDef);

    RectangleShape::setSize({10,10});
    RectangleShape::setOrigin(5,5);
    RectangleShape::setPosition(x,y);
}

Player::~Player()
{
    WorldHandler::getWorld().DestroyBody(_body);
}

void Player::Draw(sf::RenderWindow& window)
{
    RectangleShape::setPosition({_body->GetPosition().x*PIXELS_PER_METER, _body->GetPosition().y*PIXELS_PER_METER});
    window.draw(*this);
}

void Player::Update(float deltaTime)
{
    _burstCooldown += deltaTime;

    if (WindowHandler::getRenderWindow()->hasFocus())
    {
        b2Vec2 vel(0,0);

        if (sf::Keyboard::isKeyPressed((sf::Keyboard::W))){
            vel.y -= 10;
        }

        if (sf::Keyboard::isKeyPressed((sf::Keyboard::S))){
            vel.y += 10;
        }

        if (sf::Keyboard::isKeyPressed((sf::Keyboard::D))){
            vel.x += 10;
        }

        if (sf::Keyboard::isKeyPressed((sf::Keyboard::A))){
            vel.x -= 10;
        }

        vel.x = vel.x == 0 ? 0 : (vel.x > 0 ? std::max(vel.x, _body->GetLinearVelocity().x) : std::min(vel.x, _body->GetLinearVelocity().x));
        vel.y = vel.y == 0 ? 0 : (vel.y > 0 ? std::max(vel.y, _body->GetLinearVelocity().y) : std::min(vel.y, _body->GetLinearVelocity().y));
        _body->SetLinearVelocity(vel);
        if (_burstCooldown >= 1 && abs(vel.x) <= 10 && abs(vel.y) <= 10 && sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
        {
            _burstCooldown = 0;
            _body->ApplyForceToCenter({vel.x*250, vel.y*250}, true);
        }
    }
}
