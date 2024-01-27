#ifndef UPDATEMANAGER_H
#define UPDATEMANAGER_H

#pragma once

#include "UpdateInterface.hpp"

#include <list>

class UpdateInterface;

class UpdateManager
{
public:
    /// @brief called once every frame
    static void Update(float deltaTime);
    /// @brief called after update
    static void LateUpdate(float deltaTime);
    /// @brief called up to 50 times a second
    static void FixedUpdate();
    /// @brief called just before opening the window
    static void Start();

protected:
    static void addUpdateObject(UpdateInterface* obj);
    static void removeUpdateObject(UpdateInterface* obj);

    friend UpdateInterface;

private:
    inline UpdateManager() = default;

    static std::list<UpdateInterface*> _objects;
};

#endif
