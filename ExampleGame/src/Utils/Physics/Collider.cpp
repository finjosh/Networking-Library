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

b2Body* Collider::operator->()
{
    return _body;
}

const b2Body* Collider::operator->() const
{
    return _body;
}

b2Body* Collider::operator*()
{
    return _body;
}

const b2Body* Collider::operator*() const
{
    return _body;
}

b2Body* Collider::getBody()
{
    return _body;
}

const b2Body* Collider::getBody() const
{
    return _body;
}

b2Fixture* Collider::createFixture(const b2FixtureDef& fixture)
{
    return _body->CreateFixture(&fixture);
}

b2Fixture* Collider::createFixture(const b2Shape& shape, const float& density, const float& friction, 
                                    const float& restitution, const float& restitutionThreshold, const b2Filter& filter)
{
    b2FixtureDef def;
    def.density = density;
    def.friction = friction;
    def.restitution = restitution;
    def.restitutionThreshold = restitutionThreshold;
    def.filter = filter;
    def.shape = &shape;
    return _body->CreateFixture(&def);
}

b2Fixture* Collider::createFixtureSensor(const b2Shape& shape, const float& density, const b2Filter& filter)
{
    b2FixtureDef def;
    def.isSensor = true;
    def.density = density;
    def.filter = filter;
    def.shape = &shape;
    return _body->CreateFixture(&def);
}

void Collider::initCollider(const b2Vec2& pos)
{
    this->initCollider(pos.x, pos.y);
}

void Collider::initCollider(const float& x, const float& y)
{
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(x,y);
    this->initCollider(bodyDef);
}

void Collider::initCollider(const b2BodyDef& bodyDef)
{
    _body = WorldHandler::getWorld().CreateBody(&bodyDef);
    _body->GetUserData().pointer = (uintptr_t)this;
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


//* Collision Data

CollisionData::CollisionData(Collider* collider, b2Fixture* thisFixture, b2Fixture* otherFixture) : _collider(collider), _thisFixture(thisFixture), _otherFixture(otherFixture) {}

Collider* CollisionData::getCollider()
{
    return _collider;
}

b2Fixture* CollisionData::getFixtureA()
{
    return _thisFixture;
}

b2Fixture* CollisionData::getFixtureB()
{
    return _otherFixture;
}
