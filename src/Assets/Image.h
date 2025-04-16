#pragma once

#ifndef PIXELPULSE_IMAGE_H
#define PIXELPULSE_IMAGE_H

#include "../Libraries/stb.h"
#include "Platform/Std.h"
#include "IAsset.h"

namespace PixelPulse::Assets
{
    class Image : public IAsset
    {
    public:
        std::int32_t width;
        std::int32_t height;
        std::int32_t channels;
        unsigned char *data;

        Image();
        virtual ~Image();

        const char *getName() const override { return "Image"; }
        bool load() override;
        void unload() override;
        bool isLoaded() const override;
    };
}

#endif
