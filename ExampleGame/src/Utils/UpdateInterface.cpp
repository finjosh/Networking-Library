#include "Utils/UpdateInterface.hpp"
#include "Utils/UpdateManager.hpp"

UpdateInterface::UpdateInterface()
{
    UpdateManager::addUpdateObject(this);
}

UpdateInterface::~UpdateInterface()
{
    UpdateManager::removeUpdateObject(this);
}

void UpdateInterface::Update(const float& deltaTime) {}

void UpdateInterface::LateUpdate(const float& deltaTime) {}

void UpdateInterface::FixedUpdate() {}

void UpdateInterface::Start() {}
