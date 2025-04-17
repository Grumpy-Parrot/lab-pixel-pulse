#pragma once

#ifndef PIXELPULSE_ENTITY_LIBRARY_H
#define PIXELPULSE_ENTITY_LIBRARY_H

#include "IEntity.h"
#include "../Platform/Std.h"

namespace PixelPulse::Game
{
    class EntityLibrary
    {
    private:
        using EntityFactoryFunction = std::function<IEntity *()>;

        std::unordered_map<std::string, EntityFactoryFunction> m_entityFactories;

        EntityLibrary() = default;

        // No copy or move
        EntityLibrary(const EntityLibrary &) = delete;
        EntityLibrary &operator=(const EntityLibrary &) = delete;
        EntityLibrary(EntityLibrary &&) = delete;
        EntityLibrary &operator=(EntityLibrary &&) = delete;

    public:
        static EntityLibrary &getInstance()
        {
            static EntityLibrary instance;
            return instance;
        }

        bool registerEntity(const char *entityID, EntityFactoryFunction factoryFunc)
        {
            if (!entityID || !factoryFunc)
            {
                Logger::error("EntityLibrary: Attempted to register a null entity ID or factory function");
                return false;
            }

            std::string id(entityID);

            if (m_entityFactories.find(id) != m_entityFactories.end())
            {
                Logger::warning("EntityLibrary: Entity with ID '%s' is already registered", entityID);
                return false;
            }

            m_entityFactories[id] = factoryFunc;
            Logger::info("EntityLibrary: Entity registered with ID: %s", entityID);
            return true;
        }

        IEntity *createEntity(const char *entityID)
        {
            if (!entityID)
            {
                Logger::error("EntityLibrary: Attempted to create entity with null ID");
                return nullptr;
            }

            std::string id(entityID);

            auto it = m_entityFactories.find(id);
            if (it == m_entityFactories.end())
            {
                Logger::error("EntityLibrary: No entity registered with ID: %s", entityID);
                return nullptr;
            }

            return it->second();
        }

        bool isEntityRegistered(const char *entityID)
        {
            if (!entityID)
            {
                Logger::error("EntityLibrary: Attempted to check registration of null entity ID");
                return false;
            }

            std::string id(entityID);
            return m_entityFactories.find(id) != m_entityFactories.end();
        }

        std::vector<const char *> getRegisteredEntityIDs() const
        {
            std::vector<const char *> ids;
            ids.reserve(m_entityFactories.size());

            for (const auto &pair : m_entityFactories)
            {
                ids.push_back(pair.first.c_str());
            }

            return ids;
        }
    };

#define PIXELPULSE_REGISTER_ENTITY(EntityClass)                                                \
    namespace                                                                       \
    {                                                                               \
        static bool EntityClass##_registered =                                      \
            PixelPulse::Game::EntityLibrary::getInstance().registerEntity(          \
                #EntityClass,                                                       \
                []() -> PixelPulse::Game::IEntity * { return PP_NEW(EntityClass); }); \
    }
}

#endif
