#pragma once

#ifndef PIXELPULSE_ASSET_REGISTRY_H
#define PIXELPULSE_ASSET_REGISTRY_H

#include "../Platform/Std.h"
#include "IAsset.h"
#include "../Logger.h"

namespace PixelPulse::Assets
{
    struct AssetMakeRequest
    {
        const char *path;
    };

    class AssetRegistry
    {
    private:
        std::vector<IAsset *> m_assets;
        std::vector<IAsset *> m_assetsUnloadQueue;

        AssetRegistry(const AssetRegistry &) = delete;
        AssetRegistry &operator=(const AssetRegistry &) = delete;

        AssetRegistry(AssetRegistry &&) = delete;
        AssetRegistry &operator=(AssetRegistry &&) = delete;

    public:
        AssetRegistry();
        ~AssetRegistry();

        void add(IAsset *asset);
        void remove(IAsset *asset);

        // Forcefully remove an asset without waiting for it to be unloaded
        // This should be used with caution, as it may lead to dangling pointers
        void removeImmediately(IAsset *asset);

        // Unloads all assets in the unload queue
        void flushUnloadQueue();

        // Unloads all assets in active use
        void flushActiveQueue();

        template <typename T>
        T *make(AssetMakeRequest request)
        {
            static_assert(std::is_base_of<IAsset, T>::value, "T must derive from IAsset");
            Logger::info("Creating asset of type %s from path %s", typeid(T).name(), request.path);

            IAsset *existingAsset = findAsset(request.path);
            if (existingAsset)
            {
                Logger::info("Asset already exists: %s", existingAsset->getId());
                return static_cast<T *>(existingAsset);
            }

            Logger::info("Asset not found, creating new one: %s", request.path);
            T *asset = new T();
            asset->initialize(request.path);

            add(asset);
            return asset;
        }

    private:
        // Finds an asset by its path, if found on unload queue, it will be moved to active queue
        IAsset *findAsset(const char *path);
    };
}

#endif
