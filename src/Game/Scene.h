#pragma once

#ifndef PIXELPULSE_SCENE_H
#define PIXELPULSE_SCENE_H

#include "RenderPassDescriptor.h"
#include "SceneNode.h"
#include "../Platform/Std.h"

struct SDL_Renderer;

namespace PixelPulse::Game
{
    namespace PixelPulse::Assets
    {
        class AssetRegistry;
    }

    class Scene
    {
    public:
        Scene();
        Scene(const Scene &) = delete;
        virtual ~Scene();

        void setRenderer(SDL_Renderer *renderer) { this->m_renderer = renderer; }
        void setAssetRegistry(Assets::AssetRegistry *assetRegistry) { this->m_assetRegistry = assetRegistry; }

        void update(const Events::UpdateEventPayload &payload);
        void render(const Game::RenderPassDescriptor &renderPassDescriptor);

        void attach(SceneNode *node, SceneNode *parent = nullptr);
        SceneNode *spawn(IEntity *entity);
        SceneNode *spawnByID(const char *entityID);

    private:
        SDL_Renderer *m_renderer;
        Assets::AssetRegistry *m_assetRegistry;
        SceneNode *m_rootNode;
        std::vector<IEntity *> m_entities;
    };
}

#endif
