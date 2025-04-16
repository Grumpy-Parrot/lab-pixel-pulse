#include "IAsset.h"
#include "AssetRegistry.h"

using namespace PixelPulse::Assets;

AssetRegistry::AssetRegistry()
{
}

AssetRegistry::~AssetRegistry()
{
    if (!m_assets.empty())
    {
        Logger::warning("AssetRegistry is being destroyed with %zu assets still registered", m_assets.size());
        flushActiveQueue();
    }

    for (auto asset : m_assets)
    {
        delete asset;
    }
    m_assets.clear();
}

void AssetRegistry::add(IAsset *asset)
{
    if (!asset)
    {
        Logger::error("Attempted to register a null asset");
        return;
    }

    auto it = std::find(m_assets.begin(), m_assets.end(), asset);
    if (it != m_assets.end())
    {
        Logger::warning("Asset already registered");
        return;
    }

    asset->m_registry = this;

    m_assets.push_back(asset);
    Logger::info("Asset registered: %s", asset->getName());
}

void AssetRegistry::remove(IAsset *asset)
{
    Logger::debug("Removing asset '%s' (%s)", asset->getName(), asset->getId());

    if (!asset)
    {
        Logger::error("Attempted to remove a null asset");
        return;
    }

    auto it = std::find(m_assets.begin(), m_assets.end(), asset);
    if (it != m_assets.end())
    {
        m_assets.erase(it);

        auto unloadIt = std::find(m_assetsUnloadQueue.begin(), m_assetsUnloadQueue.end(), asset);
        if (unloadIt != m_assetsUnloadQueue.end())
        {
            Logger::warning("Asset '%s (%s)' is already in the unload queue", asset->getName(), asset->getId());
            return;
        }

        m_assetsUnloadQueue.push_back(asset);

        Logger::info("Asset '%s (%s)' put on unload queue", asset->getName(), asset->getId());
    }
    else
    {
        Logger::warning("Attempted to unregister an asset that is not registered");
    }
}

void AssetRegistry::removeImmediately(IAsset *asset)
{
    Logger::debug("Unregistering asset without waiting: %s", asset->getName());

    if (!asset)
    {
        Logger::error("Attempted to unregister a null asset");
        return;
    }

    auto it = std::find(m_assets.begin(), m_assets.end(), asset);
    if (it != m_assets.end())
    {
        if ((*it)->isLoaded())
        {
            (*it)->unload();
        }
        m_assets.erase(it);
        Logger::info("Asset unregistered: %s", asset->getName());
    }
    else
    {
        Logger::warning("Attempted to unregister an asset that is not registered");
    }
}

void AssetRegistry::flushUnloadQueue()
{
    Logger::info("Flushing unload queue...");

    for (auto asset : m_assetsUnloadQueue)
    {
        if (asset->isLoaded())
        {
            asset->unload();
        }
    }

    m_assetsUnloadQueue.clear();
    Logger::info("Unload queue flushed");
}

void AssetRegistry::flushActiveQueue()
{
    Logger::info("Unloading all assets...");
    int unloadedCount = 0;

    for (auto asset : m_assets)
    {
        if (asset->isLoaded())
        {
            asset->unload();
            unloadedCount++;
        }
    }

    Logger::info("Asset unloading complete. Unloaded: %d", unloadedCount);
}

IAsset *AssetRegistry::findAsset(const char *path)
{
    for (auto asset : m_assets)
    {
        if (strcmp(asset->getPath(), path) == 0)
        {
            return asset;
        }
    }

    for (auto asset : m_assetsUnloadQueue)
    {
        if (strcmp(asset->getPath(), path) == 0)
        {
            // If the asset is in the unload queue, move it to the active list
            auto it = std::find(m_assetsUnloadQueue.begin(), m_assetsUnloadQueue.end(), asset);
            if (it != m_assetsUnloadQueue.end())
            {
                m_assetsUnloadQueue.erase(it);
                m_assets.push_back(asset);
                Logger::info("Asset '%s (%s)' moved from unload queue to active list", asset->getName(), asset->getId());
            }

            return asset;
        }
    }

    return nullptr;
}
