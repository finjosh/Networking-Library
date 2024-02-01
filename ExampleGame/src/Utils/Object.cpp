#include "Utils/Object.hpp"
#include "Utils/ObjectManager.hpp"

bool _objectComp::operator()(const Object* lhs, const Object* rhs) const
{
    return lhs->getID() < rhs->getID();
}

std::atomic_ullong Object::_lastID = 0;

Object::Ptr::Ptr(Object* obj)
{
    _ptr = obj;
    if (_ptr != nullptr)
        _eventID = obj->onDestroy(Object::Ptr::removePtr, this);
}

Object::Ptr::~Ptr()
{
    if (isValid())
    {
        _ptr->onDestroy.disconnect(_eventID);
    }
}

Object* Object::Ptr::operator->() const
{
    return _ptr;
}

Object* Object::Ptr::operator*() const
{
    return _ptr;
}

Object* Object::Ptr::get()
{
    return _ptr;
}

bool Object::Ptr::isValid()
{
    return _ptr != nullptr;
}

void Object::Ptr::setObject(Object* obj)
{
    if (isValid())
    {
        _ptr->onDestroy.disconnect(_eventID);
        _ptr = nullptr;
    }

    _ptr = obj;
    _eventID = _ptr->onDestroy(Object::Ptr::removePtr, this);
}

void Object::Ptr::removePtr()
{
    _ptr = nullptr;
}


Object::Object()
{
    _id = _lastID++; //! could become an issue if lots of creation and deletion happens
    ObjectManager::addObject(this);
}

Object::Object(unsigned long long id) : _id(id) {}

Object::~Object()
{
    ObjectManager::removeObject(this);
    onDestroy.invoke();
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

Object::Ptr Object::getPtr()
{
    return Object::Ptr(this);
}
