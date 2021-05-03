#pragma once

#include <iostream>
#include <string>
#include <utility>

#define LOGGER_DEBUG(a) DefaultLogger::debug(a,__FILE__,__LINE__)
#define LOGGER_WARN(a) DefaultLogger::warn(a,__FILE__,__LINE__)
#define LOGGER_ERROR(a) DefaultLogger::error(a,__FILE__,__LINE__)

namespace PB
{
    /**
    * \brief Logging class for application logging needs.
    */
    class DefaultLogger
    {
    public:
        static void debug(const std::string& message, const std::string& filePath, int line)
        {
            log("DEBUG", message, filePath, line);
        }
        static void warn(const std::string& message, const std::string& filePath, int line)
        {
            log("WARN", message, filePath, line);
        }
        static void error(const std::string& message, const std::string& filePath, int line)
        {
            log("ERROR", message, filePath, line);
        }
    private:
        static void log(const std::string& level, const std::string& message, const std::string& filePath, int line)
        {
            time_t now;
            time(&now);
            const std::string fileName = getFileName(filePath);
            std::cout << now << " | " << level << " | " << fileName << " (" << line << "): " << message << std::endl;
        };
        static const std::string getFileName(const std::string& filePath)
        {
            std::string fileName;
            constexpr int64_t maxValue = std::numeric_limits<int64_t>::max();

            if (filePath.length() <= maxValue)
            {
                // If the first character of the string is \ then I don't care to remove it
                for (size_t i = filePath.length() - 1; i > 0; --i)
                {
                    if (filePath.at(i) == '\\')
                    {
                        fileName = filePath.substr(static_cast<size_t>(i) + 1, filePath.length() - i + 1);
                        i = 1; // Found it, we're done.
                    }
                }
            }
            else
            {
                fileName = "";
                LOGGER_WARN("File name was too long to parse");
            }

            return fileName;
        };
    };
}