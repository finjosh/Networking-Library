#include "Utils/ObjectManager.hpp"

ObjectManager::_objectCompClass ObjectManager::_compClass(0);
std::unordered_set<Object*> ObjectManager::_objects;

//* Comparison classes

ObjectManager::_objectCompClass::_objectCompClass(unsigned long long id) : Object(id) {}
void ObjectManager::_objectCompClass::setID(unsigned long long id) { Object::setID(id); }

// -------------------

Object::Ptr ObjectManager::getObject(unsigned long long id)
{
    _compClass.setID(id);
    std::unordered_set<Object*>::iterator obj = _objects.find((Object*)&_compClass);
    _compClass.setID(0);

    return Object::Ptr(obj == _objects.end() ? nullptr : *obj);
}

unsigned long long ObjectManager::getNumberOfObjects()
{
    return _objects.size();
}

Object::Ptr ObjectManager::addObject(Object* object)
{
    _objects.insert({object});
    return object->getPtr();
}

void ObjectManager::removeObject(Object* object)
{
    _objects.erase(object);
}
