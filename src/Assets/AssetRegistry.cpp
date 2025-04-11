#include "AssetRegistry.h"

AssetRegistry::AssetRegistry()
{
}

AssetRegistry::~AssetRegistry()
{
    unloadAllAssets();

    for (auto asset : assets)
    {
        delete asset;
    }
    assets.clear();
}

void AssetRegistry::registerAsset(IAsset *asset)
{
    if (!asset)
    {
        Logger::error("Attempted to register a null asset");
        return;
    }

    auto it = std::find(assets.begin(), assets.end(), asset);
    if (it != assets.end())
    {
        Logger::warning("Asset already registered");
        return;
    }

    assets.push_back(asset);
    Logger::info("Asset registered: %s", asset->getName());
}

void AssetRegistry::unregisterAsset(IAsset *asset)
{
    if (!asset)
    {
        Logger::error("Attempted to unregister a null asset");
        return;
    }

    // Find and remove the asset
    auto it = std::find(assets.begin(), assets.end(), asset);
    if (it != assets.end())
    {
        // Ensure the asset is unloaded before removing
        if ((*it)->isLoaded())
        {
            (*it)->unload();
        }
        assets.erase(it);
        Logger::info("Asset unregistered: %s", asset->getName());
    }
    else
    {
        Logger::warning("Attempted to unregister an asset that is not registered");
    }
}

void AssetRegistry::loadAllAssets()
{
    Logger::info("Loading all assets...");
    int loadedCount = 0;
    int failedCount = 0;

    for (auto asset : assets)
    {
        if (!asset->isLoaded())
        {
            if (asset->load())
            {
                loadedCount++;
            }
            else
            {
                failedCount++;
                Logger::error("Failed to load asset: %s", asset->getName());
            }
        }
    }

    Logger::info("Asset loading complete. Loaded: %d, Failed: %d", loadedCount, failedCount);
}

void AssetRegistry::unloadAllAssets()
{
    Logger::info("Unloading all assets...");
    int unloadedCount = 0;

    for (auto asset : assets)
    {
        if (asset->isLoaded())
        {
            asset->unload();
            unloadedCount++;
        }
    }

    Logger::info("Asset unloading complete. Unloaded: %d", unloadedCount);
}
