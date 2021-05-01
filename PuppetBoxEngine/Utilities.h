#pragma once

#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>

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
		/// <param name="splitValues">The resulting values of splitting the string</param>
		/// <param name="splitCount">The number of resulting values from splitting the string</param>
		/// <param name="excludeNull">If True, empty values are not added to the resulting values. Defaults to True</param>
		void split(std::string str, std::string** splitValues, uint32_t* splitCount, bool excludeNull = true);

		/// <summary>
		/// Splits the string by the given delimiter string value, limited by the split count
		/// </summary>
		/// <param name="str">The string to be split</param>
		/// <param name="delimiter">The delimiting string to split on</param>
		/// <param name="splitValues">The resulting values of splitting the string</param>
		/// <param name="splitCount">The number of resulting values from splitting the string</param>
		/// <param name="splitLimit">The number of times the string should be split max</param>
		/// <param name="excludeNull">If True, empty values are not added to the resulting values. Defaults to True</param>
		void split(std::string str, std::string delimiter, uint32_t splitLimit, std::string** splitValues, uint32_t* splitCount, bool excludeNull = true);

		/// <summary>
		/// Splits the string by the given delimiter string value
		/// </summary>
		/// <param name="str">The string to be split</param>
		/// <param name="delimiter">The delimiting string to split on</param>
		/// <param name="splitValues">The resulting values of splitting the string</param>
		/// <param name="splitCount">The number of resulting values from splitting the string</param>
		/// <param name="excludeNull">If True, empty values are not added to the resulting values. Defaults to True</param>
		void split(std::string str, std::string delimiter, std::string** splitValues, uint32_t* splitCount, bool excludeNull = true);
	};

	namespace NumberUtils
	{
		template <typename T>
		T parseValue(const char* numberAsString, T defaultValue, bool* error)
		{
			T value;

			std::stringstream stream(numberAsString);
			stream >> value;

			if (stream.fail()) {
				*error = true;
				value = defaultValue;
				LOGGER_WARN("Could not parse value '" + std::string(numberAsString) + "'");
			}

			return value;
		}
	}

	namespace FileUtils
	{
		bool getFileListFromArchive(std::string archivePath, std::unordered_set<std::string>* files);
		bool getContentsFromArchivedFile(std::string archivePath, std::string filePath, int8_t** buffer, size_t* bufferSize);
		bool getStreamFromArchivedFile(std::string archivePath, std::string filePath, std::istream** stream);
		bool getPropertiesFromStream(std::istream* stream, std::unordered_map<std::string, std::string>* properties);
	}
}