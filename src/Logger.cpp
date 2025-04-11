#include "Logger.h"

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_RESET "\x1b[0m"

Logger::Level Logger::currentLevel = Logger::Level::Info;

void Logger::setLevel(Level level)
{
    currentLevel = level;
}

void Logger::formatAndOutput(const char *prefix, const char *colorCode, const char *format, va_list args, bool isError)
{
    char buffer[1024];
    char messageBuffer[1024];

    vsnprintf(messageBuffer, sizeof(messageBuffer), format, args);

    snprintf(buffer, sizeof(buffer), "%s%s%s%s\n",
             colorCode ? colorCode : "",
             prefix ? prefix : "",
             messageBuffer,
             colorCode ? ANSI_COLOR_RESET : "");

    if (isError)
    {
        fprintf(stderr, "%s", buffer);
    }
    else
    {
        printf("%s", buffer);
    }

#ifdef PLATFORM_WINDOWS
    snprintf(buffer, sizeof(buffer), "%s%s\n",
             prefix ? prefix : "",
             messageBuffer);
    OutputDebugStringA(buffer);
#endif
}

void Logger::message(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    formatAndOutput(nullptr, nullptr, format, args, false);
    va_end(args);
}

void Logger::error(const char *format, ...)
{
    if (currentLevel <= Level::Error)
    {
        va_list args;
        va_start(args, format);
        formatAndOutput("ERROR: ", ANSI_COLOR_RED, format, args, true);
        va_end(args);
    }
}

void Logger::fatal(const char *format, ...)
{
    if (currentLevel <= Level::Error)
    {
        va_list args;
        va_start(args, format);
        formatAndOutput("FATAL: ", ANSI_COLOR_RED, format, args, true);
        va_end(args);

#ifdef _WIN32
        DebugBreak();
#else
        __builtin_trap();
#endif
    }
}

void Logger::warning(const char *format, ...)
{
    if (currentLevel <= Level::Warning)
    {
        va_list args;
        va_start(args, format);
        formatAndOutput("WARNING: ", ANSI_COLOR_YELLOW, format, args, true);
        va_end(args);
    }
}

void Logger::info(const char *format, ...)
{
    if (currentLevel <= Level::Info)
    {
        va_list args;
        va_start(args, format);
        formatAndOutput("INFO: ", ANSI_COLOR_BLUE, format, args, false);
        va_end(args);
    }
}

void Logger::debug(const char *format, ...)
{
    if (currentLevel <= Level::Debug)
    {
        va_list args;
        va_start(args, format);
        formatAndOutput("DEBUG: ", nullptr, format, args, false);
        va_end(args);
    }
}
