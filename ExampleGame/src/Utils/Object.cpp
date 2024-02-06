#include "Utils/Object.hpp"
#include "Utils/ObjectManager.hpp"

bool _objectComp::operator()(const Object* lhs, const Object* rhs) const
{
    return lhs->getID() < rhs->getID();
}

std::atomic_ullong Object::_lastID = 0;

Object::Ptr::Ptr(Object* obj)
{
    this->setObject(obj);
}

Object::Ptr::~Ptr()
{
    if (isValid())
    {
        _ptr->onDestroy.disconnect(_eventID);
    }
}

Object* Object::Ptr::operator->()
{
    return _ptr;
}

const Object* Object::Ptr::operator->() const
{
    return _ptr;
}

Object* Object::Ptr::operator*()
{
    return _ptr;
}

const Object* Object::Ptr::operator*() const
{
    return _ptr;
}

Object::Ptr& Object::Ptr::operator=(const Object::Ptr& objectPtr)
{
    this->setObject(objectPtr._ptr);
    return *this;
}

Object* Object::Ptr::get()
{
    return _ptr;
}

const Object* Object::Ptr::get() const
{
    return _ptr;
}

bool Object::Ptr::isValid() const
{
    return (_ptr != nullptr);
}

void Object::Ptr::setObject(Object* obj)
{
    if (this->isValid())
    {
        _ptr->onDestroy.disconnect(_eventID);
        _ptr = nullptr;
    }

    _ptr = obj;
    if (_ptr != nullptr)
        _eventID = _ptr->onDestroy(Object::Ptr::removePtr, this);
}

void Object::Ptr::removePtr()
{
    _ptr = nullptr;
    _eventID = 0;
}

Object::Object()
{
    _id = _lastID++; //! could become an issue if lots of creation and deletion happens
    ObjectManager::addObject(this);
}

Object::Object(unsigned long long id) : _id(id) {}

void Object::setID(unsigned long long id)
{
    _id = id;
}

Object::~Object()
{
    // this is not a real object
    if (_id == 0)
        return;
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
