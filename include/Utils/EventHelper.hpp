#ifndef EVENTHELPER_H
#define EVENTHELPER_H

#pragma once

#include <unordered_map>
#include <list>
#include <deque>
#include <memory>
#include <functional>

namespace EventHelper
{

class Event
{
public:

    virtual ~Event() 
    {
        // removing pointer to this event in thread safe in case it is still in queue
        Event::ThreadSafe::removeEvent(this);
    }

    Event() {}

    /// @brief Copy constructor which will not copy the callbacks
    Event(const Event& other);

    /// @brief Default move constructor
    Event(Event&& other) noexcept = default;

    /// @brief Copy assignment operator which will not copy the callbacks
    Event& operator=(const Event& other);

    /// @brief Default move assignment operator
    Event& operator=(Event&& other) noexcept = default;

    /// @brief Connects a function that will be called when this event is invoked
    ///
    /// @param func  Callback function that can be passed to the connect function
    /// @param args  Additional arguments to pass to the function
    ///
    /// @return Unique id of the connection (specific to this event)
    template <typename Func, typename... BoundArgs>
    unsigned int operator()(const Func& func, const BoundArgs&... args)
    {
        return connect(func, args...);
    }

    /// @brief Connects a function that will be called when this event is invoked
    ///
    /// @param func  Callback function
    /// @param args  Additional arguments to pass to the function
    ///
    /// @return Unique id of the connection
    template <typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&...)>>::value>* = nullptr>
    unsigned int connect(const Func& func, const BoundArgs&... args)
    {
        const auto id = ++m_lastId;
        if constexpr(sizeof...(BoundArgs) == 0)
            m_functions[id] = func;
        else
        {
            m_functions[id] = [=]{ invokeFunc(func, args...); };
        }
        return id;
    }

    /// @brief Disconnect a function from this event
    ///
    /// @param id  Unique id of the connection returned by the connect function
    ///
    /// @return True when a connection with this id existed and was removed
    bool disconnect(unsigned int id);

    /// @brief Disconnect all function from this event
    void disconnectAll();

    /// @brief Call all connected functions
    ///
    /// @param threadSafe if true this event will be called on EventHelper::Event::ThreadSafe::update() 
    ///
    /// @return True when at least one function was called
    bool invoke(bool threadSafe = false);

    /// @brief Calls all connected functions at: EventHelper::Event::ThreadSafe::update()
    // void invokeThreadSafe();

    /// @brief Changes whether this event calls the connected functions when triggered
    ///
    /// @param enabled  Is the event enabled?
    ///
    /// events are enabled by default. Temporarily disabling the event is the better alternative to disconnecting the
    /// handler and connecting it again a few lines later.
    void setEnabled(bool enabled)
    {
        m_enabled = enabled;
    }

    /// @brief Returns whether this event calls the connected functions when triggered
    ///
    /// @return Is the event enabled?
    ///
    /// events are enabled by default. Temporarily disabling the event is the better alternative to disconnecting the
    /// handler and connecting it again a few lines later.
    bool isEnabled() const
    {
        return m_enabled;
    }

    class ThreadSafe
    {
    public:
        /// @brief Call this first thing every frame
        static void update();
        /// @brief used when an event is destroyed in case its still in the queue
        static void removeEvent(Event* event);
        /// @brief adds the given event to the thread safe queue and copys any data needed to call it
        /// @note dont use this 
        /// @note set threadSafe = true when invoking from the event
        static void addEvent(Event* event, std::function<void()>&& func)
        {
            _events.push_back({event, func});
        }

    private:
        inline ThreadSafe() = default;

        // static std::list<std::pair<Event*, std::deque<void*>>> _events; // TODO check if making this a lambda would work
        static std::list<std::pair<Event*, std::function<void()>>> _events;
        friend Event;
    };

protected:

    void _invoke();

    /// @brief Turns the void* parameters back into its original type right before calling the callback function
    template <typename Type>
    static const std::decay_t<Type>& dereferenceParam(std::size_t paramIndex)
    {
        return *static_cast<const std::decay_t<Type>*>(m_parameters[paramIndex]);
    }

    template <typename Func, typename... Args>
    static void invokeFunc(Func&& func, Args&&... args)
    {
        std::invoke(std::forward<Func>(func), std::forward<Args>(args)...);
    }

    bool m_enabled = true;
    std::unordered_map<unsigned int, std::function<void()>> m_functions;

    // Possible issue if event are constantly removed and added
    unsigned int m_lastId;
    static std::deque<const void*> m_parameters;
};

/// Optional unbound parameters:
/// - T (value according to template parameter
template <typename T>
class EventDynamic : public Event
{
public:

    /// @brief Constructor
    EventDynamic()
    {}

    /// @brief Connects a function that will be called when this event is invoked
    ///
    /// @param func  Callback function that can be passed to the connect function
    /// @param args  Additional arguments to pass to the function
    ///
    /// @return Unique id of the connection
    template <typename Func, typename... BoundArgs>
    unsigned int operator()(const Func& func, const BoundArgs&... args)
    {
        return connect(func, args...);
    }

    /// @brief Connects a function that will be called when this event is invoked
    ///
    /// @param func  Callback function without unbound parameters
    /// @param args  Additional arguments to pass to the function
    ///
    /// @return Unique id of the connection
    template <typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&...)>>::value>* = nullptr>
    unsigned int connect(const Func& func, const BoundArgs&... args)
    {
        return Event::connect(func, args...);
    }

    /// @brief Connects a function that will be called when this event is invoked
    ///
    /// @param func  Callback function that have their last parameter unbound of type T
    /// @param args  Additional arguments to pass to the function
    ///
    /// @return Unique id of the connection
    template <typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&..., T)>>::value>* = nullptr>
    unsigned int connect(const Func& func, const BoundArgs&... args)
    {
        return Event::connect([=]{ invokeFunc(func, args..., dereferenceParam<T>(0)); });
    }

    /// @brief Call all connected callbacks
    ///
    /// @param param   Parameter that will be passed to callback function if it has an unbound parameter
    /// @param threadSafe if true this event will be called on EventHelper::Event::ThreadSafe::update() 
    ///
    /// @return True when a callback function was executed, false when there weren't any connected callback functions
    bool invoke(T param, bool threadSafe = false)
    {
        if (m_functions.empty() || !m_enabled)
            return false;

        if (threadSafe)
        {
            Event::ThreadSafe::addEvent(this, [this, param]{ Event::invokeFunc(&EventDynamic::invoke, this, param, false); });
            return true;
        }

        m_parameters[0] = static_cast<const void*>(&param);
        Event::_invoke();
        return true;
    }
};

/// Optional unbound parameters:
/// - T (value according to template parameter
template <typename T, typename T2>
class EventDynamic2 : public Event
{
public:

    /// @brief Constructor
    EventDynamic2()
    {}

    /// @brief Connects a function that will be called when this event is invoked
    ///
    /// @param func  Callback function that can be passed to the connect function
    /// @param args  Additional arguments to pass to the function
    ///
    /// @return Unique id of the connection
    template <typename Func, typename... BoundArgs>
    unsigned int operator()(const Func& func, const BoundArgs&... args)
    {
        return connect(func, args...);
    }

    /// @brief Connects a function that will be called when this event is invoked
    ///
    /// @param func  Callback function without unbound parameters
    /// @param args  Additional arguments to pass to the function
    ///
    /// @return Unique id of the connection
    template <typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&...)>>::value>* = nullptr>
    unsigned int connect(const Func& func, const BoundArgs&... args)
    {
        return Event::connect(func, args...);
    }

    /// @brief Connects a function that will be called when this event is invoked
    ///
    /// @param func  Callback function that have their last parameter unbound of type T
    /// @param args  Additional arguments to pass to the function
    ///
    /// @return Unique id of the connection
    template <typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&..., T)>>::value>* = nullptr>
    unsigned int connect(const Func& func, const BoundArgs&... args)
    {
        return Event::connect([=]{ invokeFunc(func, args..., dereferenceParam<T>(0)); });
    }

    template <typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&..., T, T2)>>::value>* = nullptr>
    unsigned int connect(const Func& func, const BoundArgs&... args)
    {
        return Event::connect([=]{ invokeFunc(func, args..., dereferenceParam<T>(0), dereferenceParam<T2>(1)); });
    }

    /// @brief Call all connected callbacks
    ///
    /// @param threadSafe if true this event will be called on EventHelper::Event::ThreadSafe::update() 
    /// @param param   Parameter that will be passed to callback function if it has an unbound parameter
    ///
    /// @return True when a callback function was executed, false when there weren't any connected callback functions
    bool invoke(T param, T2 param2, bool threadSafe = false)
    {
        if (m_functions.empty() || !m_enabled)
            return false;

        if (threadSafe)
        {
            Event::ThreadSafe::addEvent(this, [this, param, param2]{ Event::invokeFunc(&EventDynamic2::invoke, this, param, param2, false); });
            return true;
        }

        m_parameters[0] = static_cast<const void*>(&param);
        m_parameters[1] = static_cast<const void*>(&param2);
        Event::_invoke();
        return true;
    }
};

/// Optional unbound parameters:
/// - T (value according to template parameter
template <typename T, typename T2, typename T3>
class EventDynamic3 : public Event
{
public:

    /// @brief Constructor
    EventDynamic3()
    {}

    /// @brief Connects a function that will be called when this event is invoked
    ///
    /// @param func  Callback function that can be passed to the connect function
    /// @param args  Additional arguments to pass to the function
    ///
    /// @return Unique id of the connection
    template <typename Func, typename... BoundArgs>
    unsigned int operator()(const Func& func, const BoundArgs&... args)
    {
        return connect(func, args...);
    }

    /// @brief Connects a function that will be called when this event is invoked
    ///
    /// @param func  Callback function without unbound parameters
    /// @param args  Additional arguments to pass to the function
    ///
    /// @return Unique id of the connection
    template <typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&...)>>::value>* = nullptr>
    unsigned int connect(const Func& func, const BoundArgs&... args)
    {
        return Event::connect(func, args...);
    }

    /// @brief Connects a function that will be called when this event is invoked
    ///
    /// @param func  Callback function that have their last parameter unbound of type T
    /// @param args  Additional arguments to pass to the function
    ///
    /// @return Unique id of the connection
    template <typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&..., T)>>::value>* = nullptr>
    unsigned int connect(const Func& func, const BoundArgs&... args)
    {
        return Event::connect([=]{ invokeFunc(func, args..., dereferenceParam<T>(0)); });
    }

    template <typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&..., T, T2)>>::value>* = nullptr>
    unsigned int connect(const Func& func, const BoundArgs&... args)
    {
        return Event::connect([=]{ invokeFunc(func, args..., dereferenceParam<T>(0), dereferenceParam<T2>(1)); });
    }

    template <typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&..., T, T2, T3)>>::value>* = nullptr>
    unsigned int connect(const Func& func, const BoundArgs&... args)
    {
        return Event::connect([=]{ invokeFunc(func, args..., dereferenceParam<T>(0), dereferenceParam<T2>(1), dereferenceParam<T3>(2)); });
    }

    /// @brief Call all connected callbacks
    ///
    /// @param threadSafe if true this event will be called on EventHelper::Event::ThreadSafe::update() 
    /// @param param   Parameter that will be passed to callback function if it has an unbound parameter
    ///
    /// @return True when a callback function was executed, false when there weren't any connected callback functions
    bool invoke(T param, T2 param2, T3 param3, bool threadSafe = false)
    {
        if (m_functions.empty() || !m_enabled)
            return false;

        if (threadSafe)
        {
            Event::ThreadSafe::addEvent(this, [this, param, param2, param3]{ Event::invokeFunc(&EventDynamic3::invoke, this, param, param2, param3, false); });
            return true;
        }

        m_parameters[0] = static_cast<const void*>(&param);
        m_parameters[1] = static_cast<const void*>(&param2);
        m_parameters[2] = static_cast<const void*>(&param3);
        Event::_invoke();
        return true;
    }
};

/// Optional unbound parameters:
/// - T (value according to template parameter
template <typename T, typename T2, typename T3, typename T4>
class EventDynamic4 : public Event
{
public:

    /// @brief Constructor
    EventDynamic4()
    {}

    /// @brief Connects a function that will be called when this event is invoked
    ///
    /// @param func  Callback function that can be passed to the connect function
    /// @param args  Additional arguments to pass to the function
    ///
    /// @return Unique id of the connection
    template <typename Func, typename... BoundArgs>
    unsigned int operator()(const Func& func, const BoundArgs&... args)
    {
        return connect(func, args...);
    }

    /// @brief Connects a function that will be called when this event is invoked
    ///
    /// @param func  Callback function without unbound parameters
    /// @param args  Additional arguments to pass to the function
    ///
    /// @return Unique id of the connection
    template <typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&...)>>::value>* = nullptr>
    unsigned int connect(const Func& func, const BoundArgs&... args)
    {
        return Event::connect(func, args...);
    }

    /// @brief Connects a function that will be called when this event is invoked
    ///
    /// @param func  Callback function that have their last parameter unbound of type T
    /// @param args  Additional arguments to pass to the function
    ///
    /// @return Unique id of the connection
    template <typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&..., T)>>::value>* = nullptr>
    unsigned int connect(const Func& func, const BoundArgs&... args)
    {
        return Event::connect([=]{ invokeFunc(func, args..., dereferenceParam<T>(0)); });
    }

    template <typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&..., T, T2)>>::value>* = nullptr>
    unsigned int connect(const Func& func, const BoundArgs&... args)
    {
        return Event::connect([=]{ invokeFunc(func, args..., dereferenceParam<T>(0), dereferenceParam<T2>(1)); });
    }

    template <typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&..., T, T2, T3)>>::value>* = nullptr>
    unsigned int connect(const Func& func, const BoundArgs&... args)
    {
        return Event::connect([=]{ invokeFunc(func, args..., dereferenceParam<T>(0), dereferenceParam<T2>(1), dereferenceParam<T3>(2)); });
    }

    template <typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&..., T, T2, T3, T4)>>::value>* = nullptr>
    unsigned int connect(const Func& func, const BoundArgs&... args)
    {
        return Event::connect([=]{ invokeFunc(func, args..., dereferenceParam<T>(0), dereferenceParam<T2>(1), dereferenceParam<T3>(2), dereferenceParam<T4>(3)); });
    }

    /// @brief Call all connected callbacks
    ///
    /// @param threadSafe if true this event will be called on EventHelper::Event::ThreadSafe::update() 
    /// @param param   Parameter that will be passed to callback function if it has an unbound parameter
    ///
    /// @return True when a callback function was executed, false when there weren't any connected callback functions
    bool invoke(T param, T2 param2, T3 param3, T4 param4, bool threadSafe = false)
    {
        if (m_functions.empty() || !m_enabled)
            return false;

        if (threadSafe)
        {
            Event::ThreadSafe::addEvent(this, [this, param, param2, param3, param4]{ Event::invokeFunc(&EventDynamic4::invoke, this, param, param2, param3, param4, false); });
            return true;
        }

        m_parameters[0] = static_cast<const void*>(&param);
        m_parameters[1] = static_cast<const void*>(&param2);
        m_parameters[2] = static_cast<const void*>(&param3);
        m_parameters[3] = static_cast<const void*>(&param4);
        Event::_invoke();
        return true;
    }
};

/// Optional unbound parameters:
/// - T (value according to template parameter
template <typename T, typename T2, typename T3, typename T4, typename T5>
class EventDynamic5 : public Event
{
public:

    /// @brief Constructor
    EventDynamic5()
    {}

    /// @brief Connects a function that will be called when this event is invoked
    ///
    /// @param func  Callback function that can be passed to the connect function
    /// @param args  Additional arguments to pass to the function
    ///
    /// @return Unique id of the connection
    template <typename Func, typename... BoundArgs>
    unsigned int operator()(const Func& func, const BoundArgs&... args)
    {
        return connect(func, args...);
    }

    /// @brief Connects a function that will be called when this event is invoked
    ///
    /// @param func  Callback function without unbound parameters
    /// @param args  Additional arguments to pass to the function
    ///
    /// @return Unique id of the connection
    template <typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&...)>>::value>* = nullptr>
    unsigned int connect(const Func& func, const BoundArgs&... args)
    {
        return Event::connect(func, args...);
    }

    /// @brief Connects a function that will be called when this event is invoked
    ///
    /// @param func  Callback function that have their last parameter unbound of type T
    /// @param args  Additional arguments to pass to the function
    ///
    /// @return Unique id of the connection
    template <typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&..., T)>>::value>* = nullptr>
    unsigned int connect(const Func& func, const BoundArgs&... args)
    {
        return Event::connect([=]{ invokeFunc(func, args..., dereferenceParam<T>(0)); });
    }

    template <typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&..., T, T2)>>::value>* = nullptr>
    unsigned int connect(const Func& func, const BoundArgs&... args)
    {
        return Event::connect([=]{ invokeFunc(func, args..., dereferenceParam<T>(0), dereferenceParam<T2>(1)); });
    }

    template <typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&..., T, T2, T3)>>::value>* = nullptr>
    unsigned int connect(const Func& func, const BoundArgs&... args)
    {
        return Event::connect([=]{ invokeFunc(func, args..., dereferenceParam<T>(0), dereferenceParam<T2>(1), dereferenceParam<T3>(2)); });
    }

    template <typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&..., T, T2, T3, T4)>>::value>* = nullptr>
    unsigned int connect(const Func& func, const BoundArgs&... args)
    {
        return Event::connect([=]{ invokeFunc(func, args..., dereferenceParam<T>(0), dereferenceParam<T2>(1), dereferenceParam<T3>(2), dereferenceParam<T4>(3)); });
    }

    template <typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&..., T, T2, T3, T4, T5)>>::value>* = nullptr>
    unsigned int connect(const Func& func, const BoundArgs&... args)
    {
        return Event::connect([=]{ invokeFunc(func, args..., dereferenceParam<T>(0), dereferenceParam<T2>(1), dereferenceParam<T3>(2), dereferenceParam<T4>(3), dereferenceParam<T5>(4)); });
    }

    /// @brief Call all connected callbacks
    ///
    /// @param threadSafe if true this event will be called on EventHelper::Event::ThreadSafe::update() 
    /// @param param   Parameter that will be passed to callback function if it has an unbound parameter
    ///
    /// @return True when a callback function was executed, false when there weren't any connected callback functions
    bool invoke(T param, T2 param2, T3 param3, T4 param4, T5 param5, bool threadSafe = false)
    {
        if (m_functions.empty() || !m_enabled)
            return false;

        if (threadSafe)
        {
            Event::ThreadSafe::addEvent(this, [this, param, param2, param3, param4, param5]{ Event::invokeFunc(&EventDynamic5::invoke, this, param, param2, param3, param4, param5, false); });
            return true;
        }

        m_parameters[0] = static_cast<const void*>(&param);
        m_parameters[1] = static_cast<const void*>(&param2);
        m_parameters[2] = static_cast<const void*>(&param3);
        m_parameters[3] = static_cast<const void*>(&param4);
        m_parameters[4] = static_cast<const void*>(&param5);
        Event::_invoke();
        return true;
    }
};

}

#endif // EVENTHELPER_H
