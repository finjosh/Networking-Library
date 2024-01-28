#include "Utils/Object.hpp"

Object::Object()
{
    
}

void Object::setEnabled(bool enabled)
{
    _enabled = enabled;
    if (_enabled)
        onEnabled.invoke();
    else
        onDisabled.invoke();
}

bool Object::isEnabled() const
{
    return _enabled;
}

unsigned long int Object::getID() const
{
    return _id;
}
