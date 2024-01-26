#include "Utils/UpdateInterface.hpp"

UpdateInterface::UpdateInterface()
{
    UpdateManager::addUpdateObject(this);
}

UpdateInterface::~UpdateInterface()
{
    UpdateManager::removeUpdateObject(this);
}

void UpdateInterface::Update(float deltaTime) {}

void UpdateInterface::LateUpdate(float deltaTime) {}

void UpdateInterface::FixedUpdate() {}

void UpdateInterface::Start() {}
