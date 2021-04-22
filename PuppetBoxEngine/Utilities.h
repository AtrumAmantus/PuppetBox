#pragma once

#include <string>

namespace PB
{
	namespace StringUtils
	{
		/// <summary>
		/// Trims all whitespace from the beginning and the end of the string, altering the original string
		/// </summary>
		/// <param name="str">Pointer to the original string to trim</param>
		void trim(std::string* str);

		/// <summary>
		/// Trims all whitespace from the beginning and the end of the string, returning a new string
		/// </summary>
		/// <param name="str">Copy of the string to trim</param>
		/// <returns>The new trimmed string</returns>
		std::string trim(std::string str);

		/// <summary>
		/// Trims the specified character from the beginning and the end of the string, altering the original string
		/// </summary>
		/// <param name="str">Pointer to the original string to trim</param>
		/// <param name="c">The character to trim</param>
		void trim(std::string* str, const char& c);

		/// <summary>
		/// Trims the specified character from the beginning and the end of the string, returning a new string
		/// </summary>
		/// <param name="str">Copy of the string to trim</param>
		/// <param name="c">The character to trim</param>
		/// <returns>The new trimmed string</returns>
		std::string trim(std::string str, const char& c);

		/// <summary>
		/// Trims all whitespace from the beginning of the string, altering the original string
		/// </summary>
		/// <param name="str">Pointer to the original string to trim</param>
		void ltrim(std::string* str);

		/// <summary>
		/// Trims all whitespace from the beginning of the string, returning a new string
		/// </summary>
		/// <param name="str">Copy of the string to trim</param>
		/// <returns>The new trimmed string</returns>
		std::string ltrim(std::string str);

		/// <summary>
		/// Trims the specified character from the beginning of the string, altering the original string
		/// </summary>
		/// <param name="str">Pointer to the original string to trim</param>
		/// <param name="c">The character to trim</param>
		void ltrim(std::string* str, const char& c);

		/// <summary>
		/// Trims the specified character from the beginning of the string, returning a new string
		/// </summary>
		/// <param name="str">Copy of the string to trim</param>
		/// <param name="c">The character to trim</param>
		/// <returns>The new trimmed string</returns>
		std::string ltrim(std::string str, const char& c);

		/// <summary>
		/// Trims all whitespace from the end of the string, altering the original string
		/// </summary>
		/// <param name="str">Pointer to the original string to trim</param>
		void rtrim(std::string* str);

		/// <summary>
		/// Trims all whitespace from the end of the string, returning a new string
		/// </summary>
		/// <param name="str">Copy of the string to trim</param>
		/// <returns>The new trimmed string</returns>
		std::string rtrim(std::string str);

		/// <summary>
		/// Trims the specified character from the end of the string, altering the original string
		/// </summary>
		/// <param name="str">Pointer to the original string to trim</param>
		/// <param name="c">The character to trim</param>
		void rtrim(std::string* str, const char& c);

		/// <summary>
		/// Trims the specified character from the end of the string, returning a new string
		/// </summary>
		/// <param name="str">Copy of the string to trim</param>
		/// <param name="c">The character to trim</param>
		/// <returns>The new trimmed string</returns>
		std::string rtrim(std::string str, const char& c);

		/// <summary>
		/// Checks if the string starts with the given prefix
		/// </summary>
		/// <param name="str">The string to check</param>
		/// <param name="prefix">The prefix to look for</param>
		/// <returns>True if the string starts with the given prefix, False otherwise</returns>
		bool startsWith(std::string str, std::string prefix);

		/// <summary>
		/// Splits the string by all whitespace characters
		/// </summary>
		/// <param name="str">The string to be split</param>
		/// <param name="values">The resulting values of splitting the string</param>
		/// <param name="valueCount">The number of resulting values from splitting the string</param>
		/// <param name="excludeNull">If True, empty values are not added to the resulting values. Defaults to True</param>
		void split(std::string str, std::string** values, uint32_t* valueCount, bool excludeNull = true);

		/// <summary>
		/// Splits the string by the given delimiter string value
		/// </summary>
		/// <param name="str">The string to be split</param>
		/// <param name="delimiter">The delimiting string to split on</param>
		/// <param name="values">The resulting values of splitting the string</param>
		/// <param name="valueCount">The number of resulting values from splitting the string</param>
		/// <param name="excludeNull">If True, empty values are not added to the resulting values. Defaults to True</param>
		void split(std::string str, std::string delimiter, std::string** values, uint32_t* valueCount, bool excludeNull = true);
	};
}