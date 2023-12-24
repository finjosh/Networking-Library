#ifndef LIVEVAR_H
#define LIVEVAR_H

#pragma once

#include <unordered_map>
#include <string>
#include <list>
#include <numeric>

#include "SFML/Window/Event.hpp"

#include "include/Utils/funcHelper.hpp"
#include "include/Utils/EventHelper.hpp"

#define VALUE_SET std::pair<sf::Keyboard::Key, float>

class LiveVar
{
public:

    /**
     * @brief if there is not already a var of that name returns min float
    */
    static float getValue(const std::string& name);

    /**
     * @brief the given event is called whenever the value connected to the given name is changed
     * @note optional param is the value of the given var
     * @exception if there is no value with the given name returns nullptr
     * @returns a event that is called when the value is changed
    */
    static EventHelper::EventDynamic<float>* getVarEvent(const std::string& name); 

    /**
     * @brief sets the value of the var
     * @note if the given name does not exist nothing happens
     * @returns true if set false if not
    */
    static bool setValue(const std::string& name, const float& value);

    /**
     * @param func a function that will return the new value depending on the given event data
     * @note if there is already a var with the same name the new one will not be added
     * @returns false if the value was not added
    */
    static bool initVar(const std::string& name, const float& value);

    /**
     * @param func a function that will return the new value depending on the given event data
     * @note if there is already a var with the same name the new one will not be added
     * @returns false if the value was not added
    */
    static bool initVar(const std::string& name, const float& value, funcHelper::funcDynamic2<float*, sf::Event> func);

    /**
     * @param increaseKey when the given key is pressed the value will be increase by the increment amount
     * @param decreaseKey when the given key is pressed the value will be decrease by the increment amount
     * @note if there is already a var with the same name the new one will not be added
     * @returns false if the value was not added
    */
    static bool initVar(const std::string& name, const float& value, const float& increment, const sf::Keyboard::Key& increaseKey, const sf::Keyboard::Key& decreaseKey,
                        const float& min = std::numeric_limits<float>::lowest(), const float& max = std::numeric_limits<float>::max());

    /**
     * @param value is a pair with the keyboard key and the value that will be selected when said key is pressed
     * @param numberOfValues the number of pairs that will be given
     * @param v the default value if no key is clicked
     * @note if there is already a var with the same name the new one will not be added
     * @warning ellipsis must be the pair of keyboard::key, float
     * @returns false if the value was not added
    */
    inline static bool initVar(const std::string& name, const float& v, std::convertible_to<VALUE_SET> auto&&... value)
    {
        std::list<std::pair<sf::Keyboard::Key, float>> values = {value...};

        return initVar(name, v, {&presetKeys, values});
    }

    /// @returns true if removed false if not
    static bool removeVar(const std::string& name);

    static void UpdateLiveVars(const sf::Event& event);

    /// @brief called when ever a var is initalized and returns the name of the added var
    static EventHelper::EventDynamic<std::string> onVarAdded;
    /// @brief called when ever a var is removed
    static EventHelper::EventDynamic<std::string> onVarRemoved;

protected:
    // updates the value based on the keys and increment amount
    static void incrementKeys(const sf::Keyboard::Key& increaseKey, const sf::Keyboard::Key& decreaseKey, const float& increment, 
                            const float& min, const float& max, float* value, const sf::Event& event);
    // updates the value based on the preset keys and values
    static void presetKeys(const std::list<std::pair<sf::Keyboard::Key, float>>& values, float* value, const sf::Event& event);

private:
    inline LiveVar() = default;

    class var
    {
    public:
        var(const float& value, funcHelper::funcDynamic2<float*, sf::Event> keyEvent);

        float getValue() const;
        void setValue(const float& v);
        void invokeKeyEvent(const sf::Event& event);

        // called when ever the value is changed
        EventHelper::EventDynamic<float> onChanged;
    private:
        float value;
        // used to change the value based on the preset key events
        funcHelper::funcDynamic2<float*, sf::Event> keyEvent;
    };

    static std::unordered_map<std::string, var> m_vars;
};

#endif
