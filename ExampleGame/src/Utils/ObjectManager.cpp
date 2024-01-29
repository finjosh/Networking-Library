#include "Utils/ObjectManager.hpp"

std::set<Object*, _objectComp> ObjectManager::_objects;

//* Comparison classes

ObjectManager::_objectCompClass::_objectCompClass(unsigned long long id) : Object(id) {}
void ObjectManager::_objectCompClass::destroy() {}

// -------------------

Object::Ptr ObjectManager::getObject(unsigned long long id)
{
    _objectCompClass compObj(id);
    std::set<Object*>::iterator obj = _objects.find((Object*)&compObj);

    return Object::Ptr(*obj);
}

Object::Ptr ObjectManager::addObject(Object* object)
{
    _objects.insert({object});
    return Object::Ptr(object);
}

void ObjectManager::removeObject(Object* object)
{
    _objects.erase(object);
}
