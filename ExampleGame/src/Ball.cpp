#include "Ball.hpp"

Ball::Ball(const b2Vec2& pos, const b2Vec2& direction, const float& speed)
{
    b2BodyDef bodyDef;
    bodyDef.position.Set(pos.x, pos.y);
    b2Vec2 temp = direction;
    temp.Normalize();
    bodyDef.linearVelocity.Set(temp.x*speed, temp.y*speed);
    bodyDef.type = b2_kinematicBody;
    bodyDef.linearDamping = 0.f;
    bodyDef.bullet = true;
    b2CircleShape b2shape;
    b2shape.m_radius = 5;
    b2FixtureDef fixtureDef;
    fixtureDef.density = 1.0;
    fixtureDef.friction = 0.1;
    fixtureDef.restitution = 0.1;
    fixtureDef.shape = &b2shape;
    fixtureDef.isSensor = true;

    _body = WorldHandler::getWorld().CreateBody(&bodyDef);
    _body->CreateFixture(&fixtureDef);

    //* not optimal as the body could be destroyed 
    this->onEnabled(b2Body::SetEnabled, this->_body, true);
    this->onDisabled(b2Body::SetEnabled, this->_body, false);

    CircleShape::setRadius(5);
    CircleShape::setOrigin(5,5);
    CircleShape::setPosition(pos.x*PIXELS_PER_METER,pos.y*PIXELS_PER_METER);
}

Ball::~Ball()
{
    WorldHandler::getWorld().DestroyBody(_body);
}

void Ball::Update(const float& deltaTime)
{
    //! for testing
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Delete))
    {
        this->destroy();
    }
}

void Ball::Draw(sf::RenderWindow& window)
{
    CircleShape::setPosition(_body->GetPosition().x*PIXELS_PER_METER, _body->GetPosition().y*PIXELS_PER_METER);
    window.draw(*this);
}
