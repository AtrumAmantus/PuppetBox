#pragma once

#include <chrono>
#include <ctime>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "DataStructures.h"

namespace PB
{

    /**
    * \brief Utility namespace with common string related functions.
    */
    namespace StringUtils
    {
        /**
        * \brief Trims all whitespace from the beginning and the end of the string, altering the original string.
        *
        * \param Pointer to the original string to trim.
        */
        void trim(std::string* str);

        /**
        * \brief Trims all whitespace from the beginning and the end of the string, returning a new string.
        *
        * \param str	Copy of the string to trim.
        *
        * \return The new trimmed string.
        */
        std::string trim(std::string str);

        /**
        * \brief Trims the specified character from the beginning and the end of the string, altering the original string.
        *
        * \param str	Pointer to the original string to trim.
        * \param c		The character to trim.
        */
        void trim(std::string* str, const char& c);

        /**
        * \brief Trims the specified character from the beginning and the end of the string, returning a new string.
        *
        * \param str	Copy of the string to trim.
        * \param c		The character to trim.
        *
        * \return The new trimmed string.
        */
        std::string trim(std::string str, const char& c);

        /**
        * \brief Trims all whitespace from the beginning of the string, altering the original string.
        *
        * \param str	Pointer to the original string to trim.
        */
        void ltrim(std::string* str);

        /**
        * \brief Trims all whitespace from the beginning of the string, returning a new string.
        *
        * \param str	Copy of the string to trim.
        *
        * \return The new trimmed string.
        */
        std::string ltrim(std::string str);

        /**
        * \brief Trims the specified character from the beginning of the string, altering the original string.
        *
        * \param str	Pointer to the original string to trim.
        * \param c		The character to trim.
        */
        void ltrim(std::string* str, const char& c);

        /**
        * \brief Trims the specified character from the beginning of the string, returning a new string.
        *
        * \param str	Copy of the string to trim.
        * \param c		The character to trim.
        *
        * \return  The new trimmed string.
        */
        std::string ltrim(std::string str, const char& c);

        /**
        * \brief Trims all whitespace from the end of the string, altering the original string.
        *
        * \param str	Pointer to the original string to trim.
        */
        void rtrim(std::string* str);

        /**
        * \brief Trims all whitespace from the end of the string, returning a new string.
        *
        * \param str	Copy of the string to trim.
        *
        * \return The new trimmed string.
        */
        std::string rtrim(std::string str);

        /**
        * \brief Trims the specified character from the end of the string, altering the original string.
        *
        * \param str	Pointer to the original string to trim.
        * \param c		The character to trim.
        */
        void rtrim(std::string* str, const char& c);

        /**
        * \brief Trims the specified character from the end of the string, returning a new string.
        *
        * \param str	Copy of the string to trim.
        * \param c		The character to trim.
        *
        * \return The new trimmed string.
        */
        std::string rtrim(std::string str, const char& c);

        /**
        * \brief Determines if the given string starts with the given prefix.
        *
        * \param str	The string to check.
        * \param prefix The prefix to check for.
        *
        * \return True if the given string starts with the given prefix, False otherwise.
        */
        bool startsWith(const std::string& str, const std::string& prefix);

        /**
        * \brief Splits the given string by whitespace values, with an option to allow or disallow null values
        * in the array due to repeating whitespace characters in the original string.
        *
        * \param str			The original string to split.
        * \param splitValues	Pointer to the string array that will be populated with the split values.
        * \param splitCount		Pointer to the value indicating the number of elements in the array of split values.
        * \param excludeNull	If true, allows null values in the array of split values due to repeating whitespace characters.
        * in the original string, otherwise null values will be omitted.
        */
        void split(
                const std::string& str,
                std::string** splitValues,
                std::uint32_t* splitCount,
                bool excludeNull = true);

        /**
        * \brief Splits the given string by the provided delimiter value, up to a maximum limit, with an option to allow
        * or disallow null values in the array due to repeating delimiter characters in the original string.
        *
        * \param str			The original string to split.
        * \param delimiter		The delimiting value to split the string on.
        * \param splitLimit		The max number of times the original string will be split (0 means infinite).
        * \param splitValues	Pointer to the string array that will be populated with the split values.
        * \param splitCount		Pointer to the value indicating the number of elements in the array of split values.
        * \param excludeNull	If true, allows null values in the array of split values due to repeating delimiter characters.
        * in the original string, otherwise null values will be omitted.
        */
        void split(
                const std::string& str,
                const std::string& delimiter,
                std::uint32_t splitLimit,
                std::string** splitValues,
                std::uint32_t* splitCount,
                bool excludeNull = true);

        /**
        * \brief Splits the given string by the provided delimiter value, with an option to allow
        * or disallow null values in the array due to repeating delimiter characters in the original string.
        *
        * \param str			The original string to split.
        * \param delimiter		The delimiting value to split the string on.
        * \param splitValues	Pointer to the string array that will be populated with the split values.
        * \param splitCount		Pointer to the value indicating the number of elements in the array of split values.
        * \param excludeNull	If true, allows null values in the array of split values due to repeating delimiter characters.
        * in the original string, otherwise null values will be omitted.
        */
        void split(
                const std::string& str,
                const std::string& delimiter,
                std::string** splitValues,
                std::uint32_t* splitCount,
                bool excludeNull = true);

        /**
         * \brief Generates a new string with all characters transformed to lowercase.
         *
         * \param original The original string to transform.
         * \return The new string with all characters transformed to lower case equivalents.
         */
        std::string toLowerCase(const std::string& original);
    }

    /**
    * \brief Utility namespace for common Number oriented functions.
    */
    namespace NumberUtils
    {
        /**
        * \brief Parses a string value into an instance of the specified type.
        *
        * \param numberAsString	The string representation of the value to parse.
        * \param defaultValue	The default value to use if the number cannot be parsed.
        * \param error			Flag indicating an error occurred if set to True.
        *
        * \return The value parsed from the string, or the default value if it could not be parsed.
        */
        template<typename T>
        T parseValue(const char* numberAsString, T defaultValue, bool* error)
        {
            T value;

            if (numberAsString == nullptr)
            {
                *error = true;
            }
            else
            {
                std::stringstream stream(numberAsString);
                stream >> value;

                if (stream.fail())
                {
                    *error = true;
                    value = defaultValue;
                }
            }

            return value;
        }
    }

    /**
    * \brief Utility for dealing with random values.
    */
    namespace RandomUtils
    {
        /**
         * \brief Generates a random {\link PB::UUID} object.
         *
         * \return A random {\link PB::UUID} object.
         */
        UUID uuid();

        /**
        * \brief Returns values based on preloaded perlin noise values.
        *
        * \param offset	The offset value to draw from the preloaded perlin values.
        */
        float perlinValue(std::uint64_t offset);

        /**
        * \brief Returns pseudo random values with uniform distribution.
        */
        float pseudoRand();
    }
}