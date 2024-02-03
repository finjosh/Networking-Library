#include "Utils/Physics/Collider.hpp"

Collider::Collider()
{
    Object::onDisabled(&Collider::updatePhysicsState, this);
    Object::onEnabled(&Collider::updatePhysicsState, this);
}

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

void Collider::initCollider(const float& x, const float& y)
{
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(x,y);
    _body = WorldHandler::getWorld().CreateBody(&bodyDef);
}

void Collider::initCollider(const b2Vec2& pos)
{
    this->initCollider(pos.x, pos.y);
}

void Collider::initCollider(const b2BodyDef& bodyDef)
{
    _body = WorldHandler::getWorld().CreateBody(&bodyDef);
}

void Collider::setPhysicsEnabled(const bool& enabled)
{
    _enabled = enabled;

    if (_body == nullptr)
        return;

    _body->SetEnabled(_enabled && Object::isEnabled());
}

bool Collider::isPhysicsEnabled() const
{
    return _enabled && Object::isEnabled();
}

void Collider::updatePhysicsState()
{
    _body->SetEnabled(_enabled && Object::isEnabled());
}
