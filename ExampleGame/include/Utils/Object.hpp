#ifndef OBJECT_H
#define OBJECT_H

#pragma once

#include <atomic>

#include "Utils/EventHelper.hpp"

/// @brief used as a simple implementation of the pure virtual destroy function
#define createDestroy() inline void destroy() override { delete(this); };

/// @note the pure virtual "destroy" function only has to handle the destruction of the derived object
class Object
{
public:
    class Ptr
    {
    public:
        Ptr(Object* obj);
        ~Ptr();
        Object* operator->();
        const Object* operator->() const;
        Object* operator*();
        const Object* operator*() const;
        Object::Ptr& operator=(const Object::Ptr& objectPtr);
        Object* get();
        /// @brief if there is no ptr returns nullptr
        /// @returns obj or nullptr if no obj
        const Object* get() const;
        bool isValid() const;
        /// @brief assigns which obj is stored in this ptr
        /// @param obj the new obj
        void setObject(Object* obj);

    protected:
        void removePtr();

    private:
        Object* _ptr = nullptr;
        unsigned int _eventID = 0;
    };

    Object();
    ~Object();

    void setEnabled(bool enabled = true);
    bool isEnabled() const;

    unsigned long int getID() const;
    Object::Ptr getPtr();

    /// @note if derived class, use the virtual function
    EventHelper::Event onEnabled;
    /// @note if derived class, use the virtual function
    EventHelper::Event onDisabled;
    /// @note if using this in the object just use deconstructor instead
    /// @note you should NOT access any thing about the object through this event
    EventHelper::Event onDestroy;

    /// @brief tries to cast this object to a given type
    /// @returns nullptr if cast was unsuccessful  
    template<typename type>
    type* cast()
    {
        type* temp = nullptr;
    
        try
        {
            temp = dynamic_cast<type*>(this);    
        }
        catch(const std::exception& e)
        {
            temp = nullptr;
        }

        return temp;
    }

    /// @brief MUST be implemented in the final class which derives from object
    virtual void destroy() = 0;

protected:
    /// @warning only use this if you know what you are doing
    Object(unsigned long long id);
    /// @warning only use this if you know what you are doing
    void setID(unsigned long long id);
    inline virtual void OnEnable() {};
    inline virtual void OnDisable() {};

private:
    std::atomic_bool _enabled = true;
    unsigned long long _id = 0;

    static std::atomic_ullong _lastID;
};

class _objectComp
{
public:
    bool operator()(const Object* lhs, const Object* rhs) const;
};

namespace std {
    template <>
    struct hash<Object> {
        inline size_t operator()(const Object& obj) const noexcept
        {
            return hash<size_t>{}(obj.getID());
        }
    };
    template <>
    struct hash<Object*> {
        inline size_t operator()(const Object* obj) const noexcept
        {
            if (obj == nullptr)
                return 0;
            return hash<size_t>{}(obj->getID());
        }
    };
    template <>
    struct equal_to<Object> {
        inline bool operator()(const Object& obj, const Object& obj2) const noexcept
        {
            return obj.getID() == obj2.getID();
        }
    };
    template <>
    struct equal_to<Object*> {
        inline bool operator()(const Object* obj, const Object* obj2) const noexcept
        {
            return obj->getID() == obj2->getID();
        }
    };
}

#endif
