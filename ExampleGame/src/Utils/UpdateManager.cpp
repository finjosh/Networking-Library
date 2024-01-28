#include "Utils/UpdateManager.hpp"

std::list<UpdateInterface*> UpdateManager::_objects;

void UpdateManager::addUpdateObject(UpdateInterface* obj)
{
    _objects.push_back(obj);
}

void UpdateManager::removeUpdateObject(UpdateInterface* obj)
{
    _objects.remove(obj);
}

void UpdateManager::Update(float deltaTime)
{
    for (UpdateInterface* obj: _objects)
    {
        if (obj->isEnabled())
            obj->Update(deltaTime);
    }
}

void UpdateManager::LateUpdate(float deltaTime)
{
    for (UpdateInterface* obj: _objects)
    {
        if (obj->isEnabled())
            obj->LateUpdate(deltaTime);
    }
}

void UpdateManager::FixedUpdate()
{
    for (UpdateInterface* obj: _objects)
    {
        if (obj->isEnabled())
            obj->FixedUpdate();
    }
}

void UpdateManager::Start()
{
    for (UpdateInterface* obj: _objects)
    {
        obj->Start();
    }
}
