#ifndef UPDATEINTERFACE_H
#define UPDATEINTERFACE_H

#pragma once

#include "UpdateManager.hpp"

class UpdateManager;

class UpdateInterface
{
public:

    UpdateInterface();
    ~UpdateInterface();

protected:

    /// @brief called every frame
    virtual void Update(float deltaTime);
    /// @brief called every frame after update
    virtual void LateUpdate(float deltaTime);
    /// @brief called up to 50 times a second
    virtual void FixedUpdate();
    /// @brief called right before window opens
    virtual void Start();

    friend UpdateManager;

private:
    
};

#endif
