#include <iostream>

#include "Logger.h"

namespace PB
{
    namespace
    {
        static std::string getFileName(const std::string& filePath)
        {
            std::string fileName;

            if (filePath.length() <= UINT32_MAX)
            {
                // If the first character of the string is \ then I don't care to remove it
                for (std::size_t i = filePath.length() - 1; i > 0; --i)
                {
                    if (filePath.at(i) == '\\')
                    {
                        fileName = filePath.substr(static_cast<std::size_t>(i) + 1, filePath.length() - i + 1);
                        i = 1; // Found it, we're done.
                    }
                }
            }
            else
            {
                fileName = "";
                DefaultLogger::warn("File name was too long to parse", "Logger", 0);
            }

            return fileName;
        }

    }


    void DefaultLogger::log(
            const std::string& level,
            const std::string& message,
            const std::string& filePath,
            std::uint32_t line)
    {
        time_t now;
        time(&now);
        const std::string fileName = getFileName(filePath);
#       ifdef _DEBUG
        std::cout << std::to_string(now)
                  << " | " << level
                  << " | " << fileName << " (" << line << "): "
                  << message << std::endl;
#       endif
    }

    void DefaultLogger::debug(const std::string& message, const std::string& filePath, std::uint32_t line)
    {
        log("DEBUG", message, filePath, line);
    }

    void DefaultLogger::info(const std::string& message, const std::string& filePath, std::uint32_t line)
    {
        log("INFO", message, filePath, line);
    }

    void DefaultLogger::warn(const std::string& message, const std::string& filePath, std::uint32_t line)
    {
        log("WARN", message, filePath, line);
    }

    void DefaultLogger::error(const std::string& message, const std::string& filePath, std::uint32_t line)
    {
        log("ERROR", message, filePath, line);
    }
}