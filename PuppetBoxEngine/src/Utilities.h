#include <chrono>
#include <ctime>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "Logger.h"

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
                LOGGER_WARN("Null value passed to parseValue");
            }
            else
            {
                std::stringstream stream(numberAsString);
                stream >> value;

                if (stream.fail())
                {
                    *error = true;
                    value = defaultValue;
                    LOGGER_WARN("Could not parse value '" + std::string(numberAsString) + "'");
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

    /**
    * \brief Utility namespace for common file oriented functions.
    */
    namespace FileUtils
    {
        /**
        * \brief Gets list of contents of the target archive.
        *
        * \param archivePath	The path to the archive to read from.
        * \param files			A pointer to the unordered_set the file list will be saved to.
        *
        * \return True if the list was read successfully, False otherwise.
        */
        bool getFileListFromArchive(const std::string& archivePath, std::unordered_set<std::string>* files);

        /**
        * \brief Gets the raw bytes of a file inside an archive.
        *
        * \param archivePath	The path to the archive to read from.
        * \param filePath		The path to the inner file within the archive relative to it's root directory.
        * \param buffer			Pointer to the byte array that the file contents will be saved to.
        * \param bufferSize		Size of the created byte array containing the file's contents.
        *
        * \return True if the contents were read successfully, False otherwise.
        */
        bool getContentsFromArchivedFile(
                const std::string& archivePath,
                const std::string& filePath,
                std::int8_t** buffer,
                std::size_t* bufferSize);

        /**
        * \brief Gets a stream object for the contents of a given file in the given archive.
        *
        * \param archivePath	Path to the archive to read from.
        * \param filePath		Relative path to the inner file of the archive, relative to it's root directory.
        * \param stream			Pointer to the stream that will be created from the contents of the file in the archive.
        *
        * \return True if the file was read and stream created successfully, False otherwise.
        */
        bool getStreamFromArchivedFile(
                const std::string& archivePath,
                const std::string& filePath,
                std::istream** stream);
    }
}