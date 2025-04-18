#pragma once

#ifndef PIXELPULSE_STRING_TOOLS_H
#define PIXELPULSE_STRING_TOOLS_H

#include "Platform/Std.h"

namespace PixelPulse::Platform
{
    class String
    {
    public:
        static char *join(const char *a, const char *b);
        static void free(char *str)
        {
            if (str)
            {
                PP_FREE(str);
            }
        }
    };
}

#endif
