#pragma once

#ifndef PIXELPULSE_IASSET_H
#define PIXELPULSE_IASSET_H

#include "../Platform/Std.h"
#include "../Logger.h"

namespace PixelPulse::Assets
{
    class AssetRegistry;

    class IAsset
    {
    public:
        AssetRegistry *m_registry = nullptr;

        virtual ~IAsset();

        const char *getId() const { return m_id; }
        const char *getPath() const { return m_path; }
        const char *getPathAbsolute() const { return m_pathAbsolute; }

        virtual void initialize(const char *path);
        virtual bool load() = 0;
        virtual void unload() = 0;
        virtual bool isLoaded() const = 0;
        virtual const char *getName() const = 0;

        void retain();
        void release();

    protected:
        int m_refCount = 0;
        const char *m_path = nullptr;
        const char *m_pathAbsolute = nullptr;
        const char *m_id = nullptr;
    };
}

#endif
