#include "Utils/Physics/CollisionManager.hpp"

//! debugging
#include "Utils/Debug/CommandPrompt.hpp"

void CollisionListener::BeginContact(b2Contact* contact)
{
    // Command::Prompt::print("Begin contact");
    b2Body* temp = contact->GetFixtureA()->GetBody();
    if (temp != nullptr)
    {
        static_cast<CollisionCallbacks*>((void*)temp->GetUserData().pointer)->BeginContact(contact);
    }
}

void CollisionListener::EndContact(b2Contact* contact)
{
    // Command::Prompt::print("End contact");
    b2Body* temp = contact->GetFixtureA()->GetBody();
    if (temp != nullptr)
    {
        static_cast<CollisionCallbacks*>((void*)temp->GetUserData().pointer)->EndContact(contact);
    }
}

void CollisionListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
    // Command::Prompt::print("Pre solve");
    b2Body* temp = contact->GetFixtureA()->GetBody();
    if (temp != nullptr)
    {
        static_cast<CollisionCallbacks*>((void*)temp->GetUserData().pointer)->PreSolve(contact, oldManifold);
    }
}

void CollisionListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
    // Command::Prompt::print("Post solve");
    b2Body* temp = contact->GetFixtureA()->GetBody();
    if (temp != nullptr)
    {
        static_cast<CollisionCallbacks*>((void*)temp->GetUserData().pointer)->PostSolve(contact, impulse);
    }
}
