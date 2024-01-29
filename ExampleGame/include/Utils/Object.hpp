#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#pragma once

#include <atomic>

#include "Utils/EventHelper.hpp"

class Object
{
public:
    class Ptr
    {
    public:
        Ptr(Object* obj);
        ~Ptr();
        Object* operator->() const;
        Object* operator*() const;
        /// @brief if there is no ptr returns nullptr
        /// @returns obj or nullptr if no obj
        Object* get();
        bool isValid();
        void setObject(Object* obj);

    protected:
        void removePtr();

    private:
        Object* _ptr;
        unsigned int _eventID = 0;
    };

    Object();
    ~Object();

    void setEnabled(bool enabled = true);
    bool isEnabled() const;

    unsigned long int getID() const;
    Object::Ptr getPtr();

    EventHelper::Event onEnabled;
    EventHelper::Event onDisabled;
    EventHelper::Event onDestroy;

    /// @brief tries to cast this object to a given type
    /// @warning must handle obj destruction
    /// @returns nullptr if cast was unsuccessful  
    template<typename type>
    type* cast() const;

    virtual void destroy() = 0;

protected:
    /// @warning only use this if you know what you are doing
    Object(unsigned long long id);

private:
    bool _enabled = true;
    unsigned long long _id = 0;

    static std::atomic_ullong _lastID;
};

class _objectComp
{
public:
    bool operator()(const Object* lhs, const Object* rhs) const;
};

#endif