#include "Ball.hpp"

Ball::Ball(const b2Vec2& pos, const b2Vec2& direction, const float& speed) : NetworkObject(typeid(Ball).hash_code())
{
    b2CircleShape b2shape;
    b2shape.m_radius = 5/PIXELS_PER_METER;

    Collider::initCollider(pos.x,pos.y);
    Collider::createFixtureSensor(b2shape, 1);
    Collider::getBody()->SetBullet(true);

    b2Vec2 temp = direction;
    temp.Normalize();
    Collider::getBody()->SetLinearVelocity({temp.x*speed, temp.y*speed});

    CircleShape::setRadius(5);
    CircleShape::setOrigin(5,5);
    CircleShape::setPosition(pos.x*PIXELS_PER_METER,pos.y*PIXELS_PER_METER);
}

Ball::~Ball()
{

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
    CircleShape::setPosition(Collider::getBody()->GetPosition().x*PIXELS_PER_METER, Collider::getBody()->GetPosition().y*PIXELS_PER_METER);
    window.draw(*this);
}

#include "Utils/Debug/CommandPrompt.hpp"

void Ball::BeginContact(CollisionData collisionData) 
{
    // Command::Prompt::print("Ball contact begin");
}

void Ball::EndContact(CollisionData collisionData) 
{
    // Command::Prompt::print("Ball contact end");
}

void Ball::OnDataReceived(sf::Packet& data) 
{

}

sf::Packet Ball::OnSendData() 
{
    sf::Packet temp;

    return temp;
}
