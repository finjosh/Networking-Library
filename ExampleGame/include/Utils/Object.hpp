#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#pragma once

#include "Utils/EventHelper.hpp"

class Object
{
public:
    Object();

    void setEnabled(bool enabled = true);
    bool isEnabled() const;

    unsigned long int getID() const;

    EventHelper::Event onEnabled;
    EventHelper::Event onDisabled;

protected:

private:
    bool _enabled = true;
    unsigned long long _id = 0;
};

#endif
