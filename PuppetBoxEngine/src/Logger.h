#pragma once

#include <string>
#include <memory>

//TODO: Add ability to set logging level
#define LOGGER_DEBUG(a) DefaultLogger::debug(a,__FILE__,__LINE__)
#define LOGGER_INFO(a) DefaultLogger::info(a,__FILE__,__LINE__)
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
        virtual ~LoggingAppender() = default;

        virtual void send(const std::string& message) = 0;
    };

    /**
    * \brief Logging class for application logging needs.
     *
     * TODO: Add concurrency protection
    */
    class DefaultLogger
    {
    public:
        static void debug(const std::string& message, const std::string& filePath, std::uint32_t line);

        static void info(const std::string& message, const std::string& filePath, std::uint32_t line);

        static void warn(const std::string& message, const std::string& filePath, std::uint32_t line);

        static void error(const std::string& message, const std::string& filePath, std::uint32_t line);

        static void setAppender(std::unique_ptr<LoggingAppender> loggingAppender);

    private:
        static void log(
                const std::string& level,
                const std::string& message,
                const std::string& filePath,
                std::uint32_t line);
    };
}