#pragma once

#ifndef GP_IASSET_H
#define GP_IASSET_H

class IAsset
{
public:
    virtual ~IAsset() = default;

    virtual void initalize(const char* path) = 0;
    virtual bool load() = 0;
    virtual void unload() = 0;
    virtual bool isLoaded() const = 0;
    virtual const char *getName() const = 0;
};

#endif
