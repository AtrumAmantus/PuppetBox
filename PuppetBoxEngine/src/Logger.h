#pragma once

#include <string>
#include <memory>

#define LOGGER_DEBUG(a) DefaultLogger::debug(a,__FILE__,__LINE__)
#define LOGGER_WARN(a) DefaultLogger::warn(a,__FILE__,__LINE__)
#define LOGGER_ERROR(a) DefaultLogger::error(a,__FILE__,__LINE__)

namespace PB
{
    /**
     * \brief Interface for defining an appender.
     *
     * <p>Appenders offer a means to modify logging behavior, such as defining alternative
     * output targets.</p>
     */
    class LoggingAppender
    {
    public:
        virtual void send(const std::string& message) = 0;
    };

    /**
    * \brief Logging class for application logging needs.
    */
    namespace DefaultLogger
    {
        void debug(const std::string& message, const std::string& filePath, std::uint32_t line);

        void warn(const std::string& message, const std::string& filePath, std::uint32_t line);

        void error(const std::string& message, const std::string& filePath, std::uint32_t line);

        void setAppender(std::unique_ptr<LoggingAppender> loggingAppender);
    };
}