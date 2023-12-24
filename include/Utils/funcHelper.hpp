#ifndef FUNCHELPER_H
#define FUNCHELPER_H

#pragma once

#include <functional>
#include <utility>
#include <deque>

namespace funcHelper
{
template<typename _ReturnType = void>
class func
{
public:
    func() = default;
    ~func() = default;

    template<typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<_ReturnType(const BoundArgs&...)>>::value>* = nullptr>
    inline func(const Func& _function, const BoundArgs&... args)
    { 
        this->setFunction(_function, args...); 
    }

    template<typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<_ReturnType(const BoundArgs&...)>>::value>* = nullptr>
    inline void setFunction(const Func& _function, const BoundArgs&... args) 
    { 
        if constexpr(sizeof...(BoundArgs) == 0)
            this->m_function = _function;
        else
        {
            this->m_function = [=]{ return InvokeFunc(_function, args...); }; 
        }
    }

    /// @returns true, if the function held is valid (not a nullptr)
    inline bool valid() const { 
        return (bool)m_function; 
    }

    /// @brief invokes the currently set function
    inline _ReturnType invoke() const
    { 
        // auto func_copy = this->m_function; // incase the func is deleted during call
        return this->m_function();
    }

    /// @brief invokes the currently set function
    inline _ReturnType operator() () const
    { 
        return this->m_function(); 
    }

    inline const char* getTypeid() const
    { 
        return this->m_function.target_type().name();
    }

    /// @returns a copy of the bound std::function that is stored in this class
    inline std::function<_ReturnType()> getBoundFunction() const
    { 
        return m_function; 
    }

    /// @warning only compares the typeid of the function
    inline friend bool operator< (const func& _func, const func& _func2)
    { 
        return _func.getTypeid() < _func2.getTypeid(); 
    }

    /// @warning only compares the typeid of the function
    inline friend bool operator== (const func& _func, const func& _func2)
    { 
        return _func.getTypeid() == _func2.getTypeid(); 
    }

    /// @brief copies the given functions function 
    template<typename _func, typename std::enable_if_t<std::is_same<_ReturnType, std::result_of<_func()>>::value>* = nullptr, typename std::enable_if<std::is_same_v<func, _func>>>
    inline func& operator = (const _func& __func) 
    { 
        this->m_function = std::forward(__func.m_function); 
        return *this;
    }

protected:
    //* storage of the _function
    std::function<_ReturnType()> m_function;

    static std::deque<const void*> m_parameters;

    static void setMaxParameters(size_t extraParameters = 0);

    /// @brief Turns the void* parameters back into its original type right before calling the callback function
    template <typename Type>
    static const std::decay_t<Type>& dereferenceParam(std::size_t paramIndex)
    {
        return *static_cast<const std::decay_t<Type>*>(m_parameters[paramIndex]);
    }

    template<typename Func, typename... BoundArgs>
    static _ReturnType InvokeFunc(Func&& _function, BoundArgs&&... args)
    {
        return std::invoke(std::forward<Func>(_function), std::forward<BoundArgs>(args)...);
    }
};

template <typename T>
std::deque<const void*> func<T>::m_parameters;

template <typename T>
void func<T>::setMaxParameters(size_t extraParameters)
{
    if (extraParameters > m_parameters.size())
        m_parameters.resize(extraParameters);
}

/// Optional unbound parameters:
/// - T (value according to template parameter)
template <typename T>
class funcDynamic : public func<void>
{
public:

    inline funcDynamic()
    {
        func::setMaxParameters(1);
    }

    template<typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&...)>>::value ||
        std::is_convertible<Func, std::function<void(const BoundArgs&..., T)>>::value>* = nullptr>
    inline funcDynamic(const Func& _function, const BoundArgs&... args)
    { 
        func::setMaxParameters(1);
        this->setFunction(_function, args...); 
    }

    template<typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&...)>>::value>* = nullptr>
    inline void setFunction(const Func& _function, const BoundArgs&... args) 
    { 
        func::setFunction([=]{ return InvokeFunc(_function, args...); }); 
    }

    template<typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&..., T)>>::value>* = nullptr>
    inline void setFunction(const Func& _function, const BoundArgs&... args) 
    { 
        func::setFunction([=]{ return InvokeFunc(_function, args..., dereferenceParam<T>(0)); }); 
    }

    /// @brief invokes the currently set function
    inline void invoke(T param) const
    { 
        m_parameters[0] = static_cast<const void*>(&param);
        func::invoke();
    }

    /// @brief invokes the currently set function
    inline void operator() (T param)
    { 
        m_parameters[0] = static_cast<const void*>(&param);
        func::invoke(); 
    }

    /// @returns a copy of the bound std::function that is stored in this class
    inline std::function<void()> getBoundFunction() const
    { 
        return m_function; 
    }

    /// @warning only compares the typeid of the function
    inline friend bool operator< (const funcDynamic& _funcDynamic, const funcDynamic& _funcDynamic2)
    { 
        return _funcDynamic.getTypeid() < _funcDynamic2.getTypeid(); 
    }

    /// @warning only compares the typeid of the function
    inline friend bool operator== (const funcDynamic& _funcDynamic, const funcDynamic& _funcDynamic2)
    { 
        return _funcDynamic.getTypeid() == _funcDynamic2.getTypeid(); 
    }
};

/// Optional unbound parameters:
/// - T (value according to template parameter)
/// - T2 (value according to template parameter)
template <typename T, typename T2>
class funcDynamic2 : public func<void>
{
public:

    inline funcDynamic2()
    {
        func::setMaxParameters(2);
    }

    template<typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&...)>>::value ||
        std::is_convertible<Func, std::function<void(const BoundArgs&..., T)>>::value ||
        std::is_convertible<Func, std::function<void(const BoundArgs&..., T, T2)>>::value>* = nullptr>
    inline funcDynamic2(const Func& _function, const BoundArgs&... args)
    { 
        func::setMaxParameters(2);
        this->setFunction(_function, args...); 
    }

    template<typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&...)>>::value>* = nullptr>
    inline void setFunction(const Func& _function, const BoundArgs&... args) 
    { 
        func::setFunction([=]{ return InvokeFunc(_function, args...); }); 
    }

    template<typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&..., T)>>::value>* = nullptr>
    inline void setFunction(const Func& _function, const BoundArgs&... args) 
    { 
        func::setFunction([=]{ return InvokeFunc(_function, args..., dereferenceParam<T>(0)); }); 
    }

    template<typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&..., T, T2)>>::value>* = nullptr>
    inline void setFunction(const Func& _function, const BoundArgs&... args) 
    { 
        func::setFunction([=]{ return InvokeFunc(_function, args..., dereferenceParam<T>(0), dereferenceParam<T2>(1)); }); 
    }

    /// @brief invokes the currently set function
    inline void invoke(T param, T2 param2)
    { 
        m_parameters[0] = static_cast<const void*>(&param);
        m_parameters[1] = static_cast<const void*>(&param2);
        func::invoke();
    }

    /// @brief invokes the currently set function
    inline void operator() (T param, T2 param2)
    { 
        m_parameters[0] = static_cast<const void*>(&param);
        m_parameters[1] = static_cast<const void*>(&param2);
        func::invoke(); 
    }

    /// @returns a copy of the bound std::function that is stored in this class
    inline std::function<void()> getBoundFunction() const
    { 
        return m_function; 
    }

    /// @warning only compares the typeid of the function
    inline friend bool operator< (const funcDynamic2& _funcDynamic2, const funcDynamic2& _funcDynamic22)
    { 
        return _funcDynamic2.getTypeid() < _funcDynamic22.getTypeid(); 
    }

    /// @warning only compares the typeid of the function
    inline friend bool operator== (const funcDynamic2& _funcDynamic2, const funcDynamic2& _funcDynamic22)
    { 
        return _funcDynamic2.getTypeid() == _funcDynamic22.getTypeid(); 
    }
};

/// Optional unbound parameters:
/// - T (value according to template parameter)
/// - T2 (value according to template parameter)
/// - T3 (value according to template parameter)
template <typename T, typename T2, typename T3>
class funcDynamic3 : public func<void>
{
public:

    inline funcDynamic3()
    {
        func::setMaxParameters(3);
    }

    template<typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&...)>>::value ||
        std::is_convertible<Func, std::function<void(const BoundArgs&..., T)>>::value ||
        std::is_convertible<Func, std::function<void(const BoundArgs&..., T, T2)>>::value ||
        std::is_convertible<Func, std::function<void(const BoundArgs&..., T, T2, T3)>>::value>* = nullptr>
    inline funcDynamic3(const Func& _function, const BoundArgs&... args)
    { 
        func::setMaxParameters(3);
        this->setFunction(_function, args...); 
    }

    template<typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&...)>>::value>* = nullptr>
    inline void setFunction(const Func& _function, const BoundArgs&... args) 
    { 
        func::setFunction([=]{ return InvokeFunc(_function, args...); }); 
    }

    template<typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&..., T)>>::value>* = nullptr>
    inline void setFunction(const Func& _function, const BoundArgs&... args) 
    { 
        func::setFunction([=]{ return InvokeFunc(_function, args..., dereferenceParam<T>(0)); }); 
    }

    template<typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&..., T, T2)>>::value>* = nullptr>
    inline void setFunction(const Func& _function, const BoundArgs&... args) 
    { 
        func::setFunction([=]{ return InvokeFunc(_function, args..., dereferenceParam<T>(0), dereferenceParam<T2>(1)); }); 
    }

    template<typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&..., T, T2, T3)>>::value>* = nullptr>
    inline void setFunction(const Func& _function, const BoundArgs&... args) 
    { 
        func::setFunction([=]{ return InvokeFunc(_function, args..., dereferenceParam<T>(0), dereferenceParam<T2>(1), dereferenceParam<T3>(2)); }); 
    }

    /// @brief invokes the currently set function
    inline void invoke(T param, T2 param2, T3 param3)
    { 
        m_parameters[0] = static_cast<const void*>(&param);
        m_parameters[1] = static_cast<const void*>(&param2);
        m_parameters[2] = static_cast<const void*>(&param3);
        func::invoke();
    }

    /// @brief invokes the currently set function
    inline void operator() (T param, T2 param2, T3 param3)
    { 
        m_parameters[0] = static_cast<const void*>(&param);
        m_parameters[1] = static_cast<const void*>(&param2);
        m_parameters[2] = static_cast<const void*>(&param3);
        func::invoke(); 
    }

    /// @returns a copy of the bound std::function that is stored in this class
    inline std::function<void()> getBoundFunction() const
    { 
        return m_function; 
    }

    /// @warning only compares the typeid of the function
    inline friend bool operator< (const funcDynamic3& _funcDynamic3, const funcDynamic3& _funcDynamic32)
    { 
        return _funcDynamic3.getTypeid() < _funcDynamic32.getTypeid(); 
    }

    /// @warning only compares the typeid of the function
    inline friend bool operator== (const funcDynamic3& _funcDynamic3, const funcDynamic3& _funcDynamic32)
    { 
        return _funcDynamic3.getTypeid() == _funcDynamic32.getTypeid(); 
    }
};

/// Optional unbound parameters:
/// - T (value according to template parameter)
/// - T2 (value according to template parameter)
/// - T3 (value according to template parameter)
/// - T4 (value according to template parameter)
template <typename T, typename T2, typename T3, typename T4>
class funcDynamic4 : public func<void>
{
public:

    inline funcDynamic4()
    {
        func::setMaxParameters(4);
    }

    template<typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&...)>>::value ||
        std::is_convertible<Func, std::function<void(const BoundArgs&..., T)>>::value ||
        std::is_convertible<Func, std::function<void(const BoundArgs&..., T, T2)>>::value ||
        std::is_convertible<Func, std::function<void(const BoundArgs&..., T, T2, T3)>>::value ||
        std::is_convertible<Func, std::function<void(const BoundArgs&..., T, T2, T3, T4)>>::value>* = nullptr>
    inline funcDynamic4(const Func& _function, const BoundArgs&... args)
    { 
        func::setMaxParameters(4);
        this->setFunction(_function, args...); 
    }

    template<typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&...)>>::value>* = nullptr>
    inline void setFunction(const Func& _function, const BoundArgs&... args) 
    { 
        func::setFunction([=]{ return InvokeFunc(_function, args...); }); 
    }

    template<typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&..., T)>>::value>* = nullptr>
    inline void setFunction(const Func& _function, const BoundArgs&... args) 
    { 
        func::setFunction([=]{ return InvokeFunc(_function, args..., dereferenceParam<T>(0)); }); 
    }

    template<typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&..., T, T2)>>::value>* = nullptr>
    inline void setFunction(const Func& _function, const BoundArgs&... args) 
    { 
        func::setFunction([=]{ return InvokeFunc(_function, args..., dereferenceParam<T>(0), dereferenceParam<T2>(1)); }); 
    }

    template<typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&..., T, T2, T3)>>::value>* = nullptr>
    inline void setFunction(const Func& _function, const BoundArgs&... args) 
    { 
        func::setFunction([=]{ return InvokeFunc(_function, args..., dereferenceParam<T>(0), dereferenceParam<T2>(1), dereferenceParam<T3>(2)); }); 
    }

    template<typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&..., T, T2, T3, T4)>>::value>* = nullptr>
    inline void setFunction(const Func& _function, const BoundArgs&... args) 
    { 
        func::setFunction([=]{ return InvokeFunc(_function, args..., dereferenceParam<T>(0), dereferenceParam<T2>(1), dereferenceParam<T3>(2), dereferenceParam<T4>(3)); }); 
    }

    /// @brief invokes the currently set function
    inline void invoke(T param, T2 param2, T3 param3, T4 param4)
    { 
        m_parameters[0] = static_cast<const void*>(&param);
        m_parameters[1] = static_cast<const void*>(&param2);
        m_parameters[2] = static_cast<const void*>(&param3);
        m_parameters[3] = static_cast<const void*>(&param4);
        func::invoke();
    }

    /// @brief invokes the currently set function
    inline void operator() (T param, T2 param2, T3 param3, T4 param4)
    { 
        m_parameters[0] = static_cast<const void*>(&param);
        m_parameters[1] = static_cast<const void*>(&param2);
        m_parameters[2] = static_cast<const void*>(&param3);
        m_parameters[3] = static_cast<const void*>(&param4);
        func::invoke(); 
    }

    /// @returns a copy of the bound std::function that is stored in this class
    inline std::function<void()> getBoundFunction() const
    { 
        return m_function; 
    }

    /// @warning only compares the typeid of the function
    inline friend bool operator< (const funcDynamic4& _funcDynamic4, const funcDynamic4& _funcDynamic42)
    { 
        return _funcDynamic4.getTypeid() < _funcDynamic42.getTypeid(); 
    }

    /// @warning only compares the typeid of the function
    inline friend bool operator== (const funcDynamic4& _funcDynamic4, const funcDynamic4& _funcDynamic42)
    { 
        return _funcDynamic4.getTypeid() == _funcDynamic42.getTypeid(); 
    }
};

/// Optional unbound parameters:
/// - T (value according to template parameter)
/// - T2 (value according to template parameter)
/// - T3 (value according to template parameter)
/// - T4 (value according to template parameter)
/// - T5 (value according to template parameter)
template <typename T, typename T2, typename T3, typename T4, typename T5>
class funcDynamic5 : public func<void>
{
public:

    inline funcDynamic5()
    {
        func::setMaxParameters(5);
    }

    template<typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&...)>>::value ||
        std::is_convertible<Func, std::function<void(const BoundArgs&..., T)>>::value ||
        std::is_convertible<Func, std::function<void(const BoundArgs&..., T, T2)>>::value ||
        std::is_convertible<Func, std::function<void(const BoundArgs&..., T, T2, T3)>>::value ||
        std::is_convertible<Func, std::function<void(const BoundArgs&..., T, T2, T3, T4)>>::value || 
        std::is_convertible<Func, std::function<void(const BoundArgs&..., T, T2, T3, T4, T5)>>::value>* = nullptr>
    inline funcDynamic5(const Func& _function, const BoundArgs&... args)
    { 
        func::setMaxParameters(5);
        this->setFunction(_function, args...); 
    }

    template<typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&...)>>::value>* = nullptr>
    inline void setFunction(const Func& _function, const BoundArgs&... args) 
    { 
        func::setFunction([=]{ return InvokeFunc(_function, args...); }); 
    }

    template<typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&..., T)>>::value>* = nullptr>
    inline void setFunction(const Func& _function, const BoundArgs&... args) 
    { 
        func::setFunction([=]{ return InvokeFunc(_function, args..., dereferenceParam<T>(0)); }); 
    }

    template<typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&..., T, T2)>>::value>* = nullptr>
    inline void setFunction(const Func& _function, const BoundArgs&... args) 
    { 
        func::setFunction([=]{ return InvokeFunc(_function, args..., dereferenceParam<T>(0), dereferenceParam<T2>(1)); }); 
    }

    template<typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&..., T, T2, T3)>>::value>* = nullptr>
    inline void setFunction(const Func& _function, const BoundArgs&... args) 
    { 
        func::setFunction([=]{ return InvokeFunc(_function, args..., dereferenceParam<T>(0), dereferenceParam<T2>(1), dereferenceParam<T3>(2)); }); 
    }

    template<typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&..., T, T2, T3, T4)>>::value>* = nullptr>
    inline void setFunction(const Func& _function, const BoundArgs&... args) 
    { 
        func::setFunction([=]{ return InvokeFunc(_function, args..., dereferenceParam<T>(0), dereferenceParam<T2>(1), dereferenceParam<T3>(2), dereferenceParam<T4>(3)); }); 
    }

    template<typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&..., T, T2, T3, T4, T5)>>::value>* = nullptr>
    inline void setFunction(const Func& _function, const BoundArgs&... args) 
    { 
        func::setFunction([=]{ return InvokeFunc(_function, args..., dereferenceParam<T>(0), dereferenceParam<T2>(1), dereferenceParam<T3>(2), dereferenceParam<T4>(3), dereferenceParam<T5>(4)); }); 
    }

    /// @brief invokes the currently set function
    inline void invoke(T param, T2 param2, T3 param3, T4 param4, T5 param5)
    { 
        m_parameters[0] = static_cast<const void*>(&param);
        m_parameters[1] = static_cast<const void*>(&param2);
        m_parameters[2] = static_cast<const void*>(&param3);
        m_parameters[3] = static_cast<const void*>(&param4);
        m_parameters[4] = static_cast<const void*>(&param5);
        func::invoke();
    }

    /// @brief invokes the currently set function
    inline void operator() (T param, T2 param2, T3 param3, T4 param4, T5 param5)
    { 
        m_parameters[0] = static_cast<const void*>(&param);
        m_parameters[1] = static_cast<const void*>(&param2);
        m_parameters[2] = static_cast<const void*>(&param3);
        m_parameters[3] = static_cast<const void*>(&param4);
        m_parameters[4] = static_cast<const void*>(&param5);
        func::invoke(); 
    }

    /// @returns a copy of the bound std::function that is stored in this class
    inline std::function<void()> getBoundFunction() const
    { 
        return m_function; 
    }

    /// @warning only compares the typeid of the function
    inline friend bool operator< (const funcDynamic5& _funcDynamic5, const funcDynamic5& _funcDynamic52)
    { 
        return _funcDynamic5.getTypeid() < _funcDynamic52.getTypeid(); 
    }

    /// @warning only compares the typeid of the function
    inline friend bool operator== (const funcDynamic5& _funcDynamic5, const funcDynamic5& _funcDynamic52)
    { 
        return _funcDynamic5.getTypeid() == _funcDynamic52.getTypeid(); 
    }
};

}

namespace std {
    template<typename _ReturnType>
    struct hash<funcHelper::func<_ReturnType>> {
        std::size_t operator()(const funcHelper::func<_ReturnType>& func) const
        {
            // You can use the std::hash function to hash the type name of the function
            return std::hash<const char*>()(func.getTypeid());
        }
    };
}

#endif // FUNCHELPER_H
