#include "Utils/UpdateManager.hpp"

std::unordered_set<UpdateInterface*> UpdateManager::_objects;

void UpdateManager::addUpdateObject(UpdateInterface* obj)
{
    _objects.insert({obj});
}

void UpdateManager::removeUpdateObject(UpdateInterface* obj)
{
    _objects.erase(obj);
}

void UpdateManager::Update(float deltaTime)
{
    std::unordered_set<UpdateInterface*>::iterator i = _objects.begin();
    while (i != _objects.end())
    {
        // in the case that the obj is destroyed during call
        UpdateInterface* temp = *i;
        i++;
        if (temp->isEnabled())
            temp->Update(deltaTime);
    }
}

void UpdateManager::LateUpdate(float deltaTime)
{
    std::unordered_set<UpdateInterface*>::iterator i = _objects.begin();
    while (i != _objects.end())
    {
        // in the case that the obj is destroyed during call
        UpdateInterface* temp = *i;
        i++;
        if (temp->isEnabled())
            temp->LateUpdate(deltaTime);
    }
}

void UpdateManager::FixedUpdate()
{
    std::unordered_set<UpdateInterface*>::iterator i = _objects.begin();
    while (i != _objects.end())
    {
        // in the case that the obj is destroyed during call
        UpdateInterface* temp = *i;
        i++;
        if (temp->isEnabled())
            temp->FixedUpdate();
    }
}

void UpdateManager::Start()
{
    for (UpdateInterface* obj: _objects)
    {
        obj->Start();
    }
}
