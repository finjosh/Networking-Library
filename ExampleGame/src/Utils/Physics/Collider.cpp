#include "Utils/Physics/Collider.hpp"

Collider::~Collider()
{
    if (_body != nullptr)
        WorldHandler::getWorld().DestroyBody(_body);
}

b2Body* Collider::operator-> () const
{
    return _body;
}

b2Body* Collider::operator* () const
{
    return _body;
}

b2Body* Collider::getBody() const
{
    return _body;
}

b2Fixture* Collider::createFixture(const b2FixtureDef& fixture)
{
    return _body->CreateFixture(&fixture);
}

b2Fixture* Collider::createFixture(const b2Shape& shape, const float& density)
{
    return _body->CreateFixture(&shape, density);
}

void Collider::initCollider()
{
    // TODO set this up
    // _body = WorldHandler::getWorld().CreateBody();
}

void Collider::initCollider(const b2BodyDef& bodyDef)
{
    _body = WorldHandler::getWorld().CreateBody(&bodyDef);
}
