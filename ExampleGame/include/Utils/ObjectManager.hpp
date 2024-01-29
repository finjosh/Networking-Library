#ifndef OBJECTMANAGER_H
#define OBJECTMANAGER_H

#pragma once

#include <set>

#include "Utils/Object.hpp"

class ObjectManager
{
public:
    /// @returns nullptr if the object does not exist 
    static Object::Ptr getObject(unsigned long long id);

protected:
    static Object::Ptr addObject(Object* object);
    static void removeObject(Object* object);

    friend Object;

private:
    /// @note used as Object can not be made on its own due to its pure virtual destroy function
    class _objectCompClass : public Object
    {
    public:
        _objectCompClass(unsigned long long id);
        virtual void destroy() override;
    };

    static std::set<Object*, _objectComp> _objects;
};

#endif
