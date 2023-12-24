#ifndef STRINGHELPER_H
#define STRINGHELPER_H

#pragma once

#include <charconv>
#include <algorithm>
#include <string>
#include <stdexcept>

class StringHelper
{
public:
    /**
     *  @brief removes the white space from the start and end of the string
    */
    static void trim(std::string& string);
    /**
     *  @brief returns a copy of the given string without any white space in it
    */
    static std::string trim_copy(std::string string);
    static void toLower(std::string& string);
    static std::string toLower_copy(std::string string);
    /** @brief Converts the string to an integer
     *  @param result Integer value if the string contains a base 10 integer. Unmodified if string is invalid.
     *  @return Returns whether the string was valid and a value has been placed into the reference parameter.
    */ 
    static bool attemptToInt(std::string string, int& result);
    /** @brief Converts the string to an integer
     *  @param result unsigned Integer value if the string contains a base 10 unsigned integer. Unmodified if string is invalid.
     *  @return Returns whether the string was valid and a value has been placed into the reference parameter.
    */ 
    static bool attemptToUInt(std::string string, unsigned int& result);
    /** @brief Converts the string to an integer
     *  @param result Integer value if the string contains a base 10 integer. Unmodified if string is invalid.
     *  @return Returns whether the string was valid and a value has been placed into the reference parameter.
    */ 
    static bool attemptToULong(std::string string, unsigned long& result);
    /** @brief Converts the string to a float
     *  @param result  Float value if the string contains a float. Unmodified if string is invalid.
     *  @return Returns whether the string was valid and a value has been placed into the reference parameter.
    */ 
    static bool attemptToFloat(std::string string, float& result);
    /** @brief Converts the string to a float
     *  @param result  Long Double value if the string contains a Long Double. Unmodified if string is invalid.
     *  @return Returns whether the string was valid and a value has been placed into the reference parameter.
    */ 
    static bool attemptToLongDouble(std::string string, long double& result);
    /** @brief Converts the string to a bool
     *  @param result  Bool Value if the string contains a bool. Unmodified if string is invalid.
     *  @return Returns whether the string was valid and a value has been place into the reference parameter.
    */ 
    static bool attemptToBool(std::string string, bool& result);
    /** @brief Converts the string to an integer
     *  @param defaultValue  Value to return if conversion fails
     *  @return Returns the integer value or defaultValue if the string didn't contain a base 10 integer
    */ 
    static int toInt(const std::string& string, const int& defaultValue = 0);
    /** @brief Converts the string to an integer
     *  @param defaultValue  Value to return if conversion fails
     *  @return Returns the integer value or defaultValue if the string didn't contain a base 10 integer
    */ 
    static unsigned int toUInt(const std::string& string, const unsigned int& defaultValue = 0); 
    /** @brief Converts the string to an integer
     *  @param defaultValue  Value to return if conversion fails
     *  @return Returns the integer value or defaultValue if the string didn't contain a base 10 integer
    */ 
   static unsigned long toULong(const std::string& string, const int& defaultValue = 0);
    /** @brief Converts the string to a float
     *  @param defaultValue  Value to return if conversion fails
     *  @return Returns the float value or defaultValue if the string didn't contain a float
    */ 
    static float toFloat(const std::string& string, const float& defaultValue = 0);
    /** @brief Converts the string to a long double
     *  @param defaultValue  Value to return if conversion fails
     *  @return Returns the long double value or defaultValue if the string didn't contain a long double
    */ 
    static long double toLongDouble(const std::string& string, const long double& defaultValue = 0);
    /** @brief Converts the string to a bool
     *  @param defaultValue  Value to return if conversion fails
     *  @return Returns the bool value or defaultValue if the string didn't contain a bool
    */ 
    static bool toBool(const std::string& string, const bool& defaultValue = 0);
    /** @brief Converts a float to a string while rounding its decimal value
     *  @param value is the float value to be converted
     *  @param decimalRoundingPlace is the number of decimals to round to
     *  @warning max rounding of 6
    */
    static std::string FloatToStringRound(float value, unsigned int decimalRoundingPlace = 1);

private:
    inline StringHelper() = default;
    static const char *const whitespaceDelimiters;
};

#endif
