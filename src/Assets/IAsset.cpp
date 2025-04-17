#include "IAsset.h"
#include "../Assets/AssetRegistry.h"
#include "../Platform/String.h"
#include "Libraries/Libraries.h"
#include "../Utilities.h"

using namespace PixelPulse::Assets;

IAsset::~IAsset()
{
    if (m_pathAbsolute)
    {
        PP_FREE((void *)m_pathAbsolute);
        m_pathAbsolute = nullptr;
    }

    PIXELPULSE_FREE_ID(m_id);
}

void IAsset::initialize(const char *path)
{
    if (m_pathAbsolute)
    {
        PP_FREE((void *)m_pathAbsolute);
    }

    const char *appPath = SDL_GetBasePath();

    m_path = path;
    m_pathAbsolute = Platform::String::join(appPath, m_path);

    PIXELPULSE_FREE_ID(m_id);

    m_id = PIXELPULSE_MAKE_ID_DERIVED(m_path);
}

void IAsset::retain()
{
    m_refCount++;
    Logger::debug("Asset '%s' (%s) retained, new ref count: %d", getName(), getId(), m_refCount);
}

void IAsset::release()
{
    m_refCount--;
    Logger::debug("Asset '%s' (%s) released, new ref count: %d", getName(), getId(), m_refCount);
    if (m_refCount <= 0)
    {
        Logger::debug("Asset '%s' (%s) reached zero reference count, unregistering", getName(), getId());
        if (m_registry)
        {
            Logger::debug("Removing asset '%s' (%s) from registry", getName(), getId());
            m_registry->remove(this);
        }
        else
        {
            Logger::error("AssetRegistry is null, cannot unregister asset!");
        }
    }
}
