#ifndef COMMANDHANDLER_H
#define COMMANDHANDLER_H

#pragma once

#include "include/Utils/funcHelper.hpp"
#include "include/Utils/StringHelper.hpp"

#include <map>
#include <vector>
#include <string>
#include <bits/stdc++.h>

namespace Command
{

/// @brief 4 space string
#define TAB_STR "    "
#define Tokens std::vector<std::string>

struct color
{
    /// @brief default color is white 
    inline color(uint8_t r = 255, uint8_t g = 255, uint8_t b = 255, uint8_t a = 255) :
        r(r), b(b), g(g), a(a)
    {}

    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

const color WARNING_COLOR = color(255,164,0,255);
const color ERROR_COLOR = color(255,0,0,255);
const color INVALID_INPUT_COLOR = color(255,164,0,255);

class Data
{
public:
    /// @brief sets the tokens with the given command
    /// @note if empty then starts with no tokens
    Data(const std::string& command = "");
    /// @brief same as setTokens
    void setCommand(const std::string& command);

    /// @brief sets the tokens given a string
    /// @note parses the string for the tokens 
    /// @note tokens will contain the command unless the data is given through the command call
    void setTokens(const std::string& command);
    /// @brief overrides the current tokens to the given tokens
    void setTokens(const Tokens& tokens);
    void setToken(const size_t& index, const std::string& tokenStr);
    void addToken(const std::string& tokenStr);
    void removeToken(const size_t& index = 0);

    /// @returns a const reference to the current tokens
    const Tokens& getTokens();
    /// @param index the index of the wanted token
    /// @returns if there is a token at the index returns token else returns a string with nothing
    const std::string getToken(const size_t& index = 0);
    /// @returns the current number of tokens stored
    size_t getNumOfTokens();

    const color& getReturnColor();
    /// @brief sets the wanted color of the return string
    /// @note is handled by command prompt
    void setReturnColor(const color& color);

    /// @returns the current return string
    const std::string& getReturnStr();
    /// @brief sets the string that will be returned after the command if finished
    void setReturnStr(const std::string& returnStr);
    /// @brief adds a line to the return string
    /// @param line the line to be added (no \\n needed)
    void addReturnStrLine(const std::string& line);
    /// @brief adds a string to the return string
    /// @note does not add the stirng in a new line
    /// @param str the string to be added
    void addToReturnStr(const std::string& str);

    /// @brief parses and calls commands that are being used for input to the main command
    /// @note should only be used if you are going to call the command that is set
    void deepParseCommand();

protected:

    /**
     * @brief parses the given command
     * @warning if not able to be parsed returns a empty list
    */
    static Tokens parseCommand(const std::string& command);

private:
    color _color;
    Tokens _tokens;
    std::string _return;
};

/// @brief tries to convert the given string to the given type
/// @typedef Compare a function that is called with the converted value to check if it is still valid
/// @param errorMsg the msg that will be put into data if the input is invalid
/// @param data the data which and invalid input statement will be place if not able to convert
/// @param value the value wanted
/// @param strValue the value as a string
/// @param defaultValue the value will be assigned this value is the string is invalid
/// @param comparisonFunc if the value does not fit this comparison then the input will no longer be valid
/// @note also changes the return color to invalid input color if needed
/// @returns false if the input is not valid for the given type 
template<typename valueType, typename Compare = bool(valueType&), typename std::enable_if_t<std::is_same_v<valueType, int> || std::is_same_v<valueType, float> || 
                                std::is_same_v<valueType, unsigned long> || std::is_same_v<valueType, unsigned int> || std::is_same_v<valueType, bool>>* = nullptr>
inline bool isValidInput(const std::string& errorMsg, Data& data, const std::string& strValue, 
                        valueType& value, const valueType& defaultValue, Compare comp = [](valueType& value){ return true; })
{
    bool valid = false;

    if constexpr (std::is_same_v<valueType, int>)
    {
        valid = StringHelper::attemptToInt(strValue, value);
    }
    else if constexpr (std::is_same_v<valueType, float>)
    {
        valid = StringHelper::attemptToFloat(strValue, value);
    }
    else if constexpr (std::is_same_v<valueType, unsigned int>)
    {
        valid = StringHelper::attemptToUInt(strValue, value);
    }
    else if constexpr (std::is_same_v<valueType, bool>)
    {
        valid = StringHelper::attemptToBool(strValue, value);
    }
    else // if the type is unsigned long 
    {
        valid = StringHelper::attemptToULong(strValue, value);
    }

    if (!valid || !comp(value))
    {
        valid = false;
        data.setReturnStr(errorMsg);
        data.setReturnColor(Command::INVALID_INPUT_COLOR);
        value = defaultValue;
    }

    return valid;
}

/// @brief template for printing something (can be used as the func for a command)
/// @note only give a value for str
void print(const std::string& str, Data* input);

class Handler;

class command
{
public:

    /// @param name name of the command
    /// @param description description of the command
    /// @param func the function to be called can take in Command Data if wanted
    /// @param subCommands sub commands that are required after the current function
    command(const std::string& name, const std::string& description, 
            const funcHelper::funcDynamic<Data*>& func = [](){}, const std::list<command>& subCommands = {});
    command(const command& command);

    bool operator< (const command& command) const;
    bool operator< (const std::string& str) const;
    bool operator> (const command& command) const;
    bool operator> (const std::string& str) const;
    bool operator== (const command& command) const;
    bool operator== (const std::string& str) const;

    std::string getName() const;
    std::string getDescription() const;
    /// @returns the name separated by " - " and the description
    std::string getNameDescription(const size_t& maxLength = SIZE_MAX) const;
    /// @param subCommandIndex is only for the recursive usage of this function
    /// @note does not add the current commands name and description
    /// @returns the name separated by " - " and the description of each sub command (with the max length given)
    std::string getSubCommandsNameDescription(const size_t& maxLength = SIZE_MAX, size_t subCommandIndex = 1) const;

    /// @note data should only contain inputs not command names
    void invoke(Data& data);

protected:
    std::list<command> _subCommands;

private:
    std::string _name;
    std::string _description;
    
    funcHelper::funcDynamic<Data*> _function;

    friend Handler; // allowing Handler to access private and protected vars
};

class Handler
{
public:
    /// @brief if there already exists a command that was givin it will not be added
    /// @note if there exists a command that was added but its sub commands are not already added it will be added
    static void addCommand(const command& command);

    /**
     * @brief used for auto fill / command predictions
     * @param search will take in any str and try to parse it (tokens being separated by spaces)
     * @note if there is a space after the commend the auto fill will search for the next commands if possible
     * @returns a sorted list of possible commands
    */
    static std::list<std::string> autoFillSearch(const std::string& search);

    /**
     * @brief calls the given command
     * @param command will take in any str and try to parse it (tokens being separated by spaces)
     * @returns pair of string and color (string should be printed in the given color)
    */
    static Data callCommand(const std::string& commandStr);

protected:
    /// @param cmd the command that is wanted to be added
    /// @param _commands the commands we are trying to add to
    static void _addCommand(const command& cmd, std::list<command>& _commands);

private:
    inline Handler() = default;

    static std::list<command> _commands;
};

}

#endif
