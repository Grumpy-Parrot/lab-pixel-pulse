#include "Image.h"
#include "Logger.h"
#include "Platform/Platform.h"
#include "StringTools.h"

Image::Image() : width(0), height(0), channels(0), data(nullptr), pathAbsolute(nullptr) {}

Image::~Image()
{
    if (data)
    {
        stbi_image_free(data);
    }
    if (pathAbsolute)
    {
        std::free((void *)pathAbsolute);
    }
}

void Image::initalize(const char *path)
{
    const char *appPath = SDL_GetBasePath();
    pathAbsolute = CString::join(appPath, path);
}

bool Image::load()
{
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
        Logger::info("Unloaded image: %s", pathAbsolute);
    }
}

bool Image::isLoaded() const
{
    return data != nullptr;
}
