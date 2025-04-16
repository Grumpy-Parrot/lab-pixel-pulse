#include "Logger.h"
#include "Platform/Platform.h"
#include "Platform/String.h"
#include "Image.h"

using namespace PixelPulse::Assets;

Image::Image() : width(0),
                 height(0),
                 channels(0),
                 data(nullptr)
{
}

Image::~Image()
{
    if (data)
    {
        stbi_image_free(data);
    }
}

bool Image::load()
{
    const char *pathAbsolute = getPathAbsolute();
    data = stbi_load(pathAbsolute, &width, &height, &channels, 4); // Force RGBA

    if (!data)
    {
        Logger::error("Failed to load image: %s", stbi_failure_reason());
        return false;
    }
    else
    {
        Logger::info("Loaded image: %s (%dx%d)", pathAbsolute, width, height);
    }

    return true;
}

void Image::unload()
{
    if (data)
    {
        stbi_image_free(data);
        data = nullptr;
    }
    else
    {
        Logger::warning("Image already unloaded: %s", getPath());
    }
}

bool Image::isLoaded() const
{
    return data != nullptr;
}
