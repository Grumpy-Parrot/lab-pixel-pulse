#pragma once

#ifndef GP_IMAGE_H
#define GP_IMAGE_H

#include "Libraries/Libraries.h"
#include "Platform/Std.h"
#include "IAsset.h"

class Image : public IAsset
{
public:
    int width;
    int height;
    int channels;
    unsigned char* data;
    const char* pathAbsolute;

    Image();
    ~Image();

    const char* getName() const override { return "Image"; }
    bool load() override;
    void unload() override;
    bool isLoaded() const override;
    void initalize(const char* path) override;
};

#endif
