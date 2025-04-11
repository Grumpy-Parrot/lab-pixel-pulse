#pragma once

#ifndef GP_ASSET_REGISTRY_H
#define GP_ASSET_REGISTRY_H

#include "../Platform/Std.h"
#include "IAsset.h"
#include "../Logger.h"

struct AssetMakeRequest
{
    const char *path;
};

class AssetRegistry
{
private:
    std::vector<IAsset *> assets;

    AssetRegistry(const AssetRegistry &) = delete;
    AssetRegistry &operator=(const AssetRegistry &) = delete;

    AssetRegistry(AssetRegistry &&) = delete;
    AssetRegistry &operator=(AssetRegistry &&) = delete;

public:
    AssetRegistry();
    ~AssetRegistry();

    void registerAsset(IAsset *asset);
    void unregisterAsset(IAsset *asset);
    void loadAllAssets();
    void unloadAllAssets();

    template <typename T>
    T *make(AssetMakeRequest request)
    {
        static_assert(std::is_base_of<IAsset, T>::value, "T must derive from IAsset");
        Logger::info("Creating asset of type %s from path %s", typeid(T).name(), request.path);

        T *asset = new T();
        asset->initalize(request.path);

        registerAsset(asset);

        return asset;
    }
};

#endif
