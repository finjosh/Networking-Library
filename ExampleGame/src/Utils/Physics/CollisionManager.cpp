#include "Utils/Physics/CollisionManager.hpp"

//! debugging
#include "Utils/Debug/CommandPrompt.hpp"

void CollisionListener::BeginContact(b2Contact* contact)
{
    

    b2Body* temp = contact->GetFixtureA()->GetBody();
    if (temp != nullptr)
    {
        static_cast<CollisionCallbacks*>((void*)temp->GetUserData().pointer)->BeginContact(contact);
    }
    temp = contact->GetFixtureB()->GetBody();
    if (temp != nullptr)
    {
        static_cast<CollisionCallbacks*>((void*)temp->GetUserData().pointer)->BeginContact(contact);
    }
}

void CollisionListener::EndContact(b2Contact* contact)
{
    b2Body* temp = contact->GetFixtureA()->GetBody();
    if (temp != nullptr)
    {
        static_cast<CollisionCallbacks*>((void*)temp->GetUserData().pointer)->EndContact(contact);
    }
    temp = contact->GetFixtureB()->GetBody();
    if (temp != nullptr)
    {
        static_cast<CollisionCallbacks*>((void*)temp->GetUserData().pointer)->EndContact(contact);
    }
}

void CollisionListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
    b2Body* temp = contact->GetFixtureA()->GetBody();
    if (temp != nullptr)
    {
        static_cast<CollisionCallbacks*>((void*)temp->GetUserData().pointer)->PreSolve(contact, oldManifold);
    }
    temp = contact->GetFixtureB()->GetBody();
    if (temp != nullptr)
    {
        static_cast<CollisionCallbacks*>((void*)temp->GetUserData().pointer)->PreSolve(contact, oldManifold);
    }
}

void CollisionListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
    b2Body* temp = contact->GetFixtureA()->GetBody();
    if (temp != nullptr)
    {
        static_cast<CollisionCallbacks*>((void*)temp->GetUserData().pointer)->PostSolve(contact, impulse);
    }
    temp = contact->GetFixtureB()->GetBody();
    if (temp != nullptr)
    {
        static_cast<CollisionCallbacks*>((void*)temp->GetUserData().pointer)->PostSolve(contact, impulse);
    }
}
