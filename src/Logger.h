#pragma once

#include "Platform/Platform.h"
#include "Platform/Std.h"

class Logger {
public:
    enum class Level {
        Debug,
        Info,
        Warning,
        Error
    };

    static void setLevel(Level level);
    static void message(const char* format, ...);
    static void error(const char* format, ...);
    static void fatal(const char* format, ...);
    static void warning(const char* format, ...);
    static void info(const char* format, ...);
    static void debug(const char* format, ...);

private:
    static Level currentLevel;
    static void formatAndOutput(const char* prefix, const char* colorCode, const char* format, va_list args, bool isError);
};
