#pragma once

#ifndef PIXELPULSE_RENDERPASSDESCRIPTOR_H
#define PIXELPULSE_RENDERPASSDESCRIPTOR_H

#include "Platform/Std.h"
#include "Math/Vector2.h"

namespace PixelPulse::Game
{
    struct RenderPassDescriptor
    {
        Math::Vector2<std::int32_t> windowSize;
    };
}

#endif
