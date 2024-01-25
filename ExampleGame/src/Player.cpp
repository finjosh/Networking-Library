#include "Player.hpp"

Player::Player(float x, float y)
{
    b2BodyDef bodyDef;
    bodyDef.position.Set(x/PIXELS_PER_METER, y/PIXELS_PER_METER);
    bodyDef.type = b2_dynamicBody;
    bodyDef.linearDamping = 10;
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

void Player::draw(sf::RenderStates rs)
{
    RectangleShape::setPosition({_body->GetPosition().x*PIXELS_PER_METER, _body->GetPosition().y*PIXELS_PER_METER});
    WindowHandler::getRenderWindow()->draw(*this, rs);
}

void Player::updateInput()
{
    if (WindowHandler::getRenderWindow()->hasFocus())
    {
        if (sf::Keyboard::isKeyPressed((sf::Keyboard::W))){
            _body->SetLinearVelocity({_body->GetLinearVelocity().x,-10});
        }

        if (sf::Keyboard::isKeyPressed((sf::Keyboard::S))){
            _body->SetLinearVelocity({_body->GetLinearVelocity().x,10});
        }

        if (sf::Keyboard::isKeyPressed((sf::Keyboard::D))){
            _body->SetLinearVelocity({10,_body->GetLinearVelocity().y});
        }

        if (sf::Keyboard::isKeyPressed((sf::Keyboard::A))){
            _body->SetLinearVelocity({-10,_body->GetLinearVelocity().y});
        }
    }
}
