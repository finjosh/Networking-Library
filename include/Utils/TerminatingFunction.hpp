#ifndef TERMINATINGFUNCTION_H
#define TERMINATINGFUNCTION_H

#pragma once

#include <list>
#include <string>

#include "funcHelper.hpp"
#include "EventHelper.hpp"

#if __has_include("include/Utils/Debug/CommandHandler.hpp")
#include "include/Utils/Debug/CommandHandler.hpp" 
#endif
#if __has_include("include/Utils/Debug/CommandPrompt.hpp")
#include "include/Utils/Debug/CommandPrompt.hpp"
#endif

struct TerminatingFunction 
{
public:
    enum State
    {
        Finished = 0,
        Running = 1
    };

    struct data
    {
        inline data(const float& deltaTime, const float& totalTime) : deltaTime(deltaTime), totalTime(totalTime) {};
        const float deltaTime = 0.f;
        const float totalTime = 0.f;
        State state = State::Finished;

        inline void setRunning() { state = State::Running; }
        /// @brief NOT required as state is by default finished
        inline void setFinished() { state = State::Finished; }
    };

    /// @brief Calling this function will call all Terminating Functions and erase ther from the list if they are finished running
    /// @param deltaTime the deltaTime in seconds
    static void UpdateFunctions(float deltaTime);
    /// @note if the function is already added it will be added again
    /// @warning The function will only be added if it is valid
    /// @returns the function typeid is "" if function is not valid
    static std::string Add(funcHelper::funcDynamic<data*> function);
    /// @brief clears all terminating functions from the list 
    static void clear();
    /// @brief erases the given function from the terminating functions list IF there is one in the list
    /// @warning removes all functions with the same functionTypeid
    static void remove(const std::string& functionTypeid);

    /// @brief gets the string data from ever terminating function at the moment
    /// @return a list where each item is a pair of, name and total time
    static std::list<std::pair<std::string, std::string>> getStringData();

    #ifdef COMMANDPROMPT_H
        /// @brief prints the string data to the command prompt every frame
        /// @param print if turn prints every frame
        static void printToPrompt(const bool& print = false);
        /// @brief prints the string data to the command prompt once
        static void printToPromptOnce();
    #endif

    #ifdef COMMANDHANDLER_H
        // must be called separately as static classes are not initalized in any specific order
        static void initCommands();
    #endif 

protected:
    struct _tFunc
    {
        inline _tFunc(funcHelper::funcDynamic<data*> func) : func(func) {};

        funcHelper::funcDynamic<data*> func;
        float totalTime = 0.f;

        inline bool operator== (const _tFunc& tFunc)
        {
            return func.getTypeid() == tFunc.func.getTypeid();
        }

        /// @brief removes everything before "setFunction"
        /// @param typeID the typeID of the function
        /// @returns shortened id
        inline std::string getShortenedID(const std::string& typeID)
        {
            return typeID.substr(typeID.find("setFunction")+11);
        }

        /// @return pair of strings first is name and second is total time
        inline std::pair<std::string, std::string> toString()
        {
            return {getShortenedID(func.getTypeid()), std::to_string(totalTime)};
        }
    };

    // second value is the current time the function has been running for
    static std::list<_tFunc> terminatingFunctions;
private:
    inline TerminatingFunction() = default;
    #ifdef COMMANDPROMPT_H
    static bool _printToPrompt;
    #endif
};

typedef TerminatingFunction TFunc;
typedef funcHelper::funcDynamic<TFunc::data*> TFunction;
typedef TerminatingFunction::data TData;
typedef TerminatingFunction::State TState;

#endif // TERMINATINGFUNCTION_H
