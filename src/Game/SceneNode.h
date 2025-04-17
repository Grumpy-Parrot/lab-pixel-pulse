#pragma once

#ifndef PIXELPULSE_SCENENODE_H
#define PIXELPULSE_SCENENODE_H

#include "../Platform/Std.h"
#include "../Math/Vector2.h"
#include "Events/UpdateEventPayload.h"
#include "IEntity.h"

struct SDL_Renderer;

namespace PixelPulse::Game
{
    class Sprite;
    struct RenderPassDescriptor;

    class SceneNode
    {
    public:
        Math::Vector2<float> m_position;
        Math::Vector2<float> m_scale;
        float m_rotation; // In degrees

        SceneNode();
        virtual ~SceneNode();

        void addChild(SceneNode *child);

        void setSprite(Sprite *sprite);
        Sprite *getSprite() const { return m_sprite; }

        void setEntity(IEntity *entity);
        IEntity *getEntity() const { return m_entity; }

        void setTag(const char *tag);
        const char *getTag() const { return m_tag; }

        void update(const Events::UpdateEventPayload &payload, const Math::Vector2<float> &parentPosition, const Math::Vector2<float> &parentScale);
        void render(::SDL_Renderer *renderer, const Game::RenderPassDescriptor *renderPassDescriptor);

    private:
        Sprite *m_sprite;
        IEntity *m_entity;
        SceneNode *m_parent;
        const char *m_tag;

        Math::Vector2<float> m_worldPosition;
        Math::Vector2<float> m_worldScale;

    public:
        std::vector<SceneNode *> m_children;

        // Prevent copying
        SceneNode(const SceneNode &) = delete;
        SceneNode &operator=(const SceneNode &) = delete;

        // Allow moving
        SceneNode(SceneNode &&) = default;
        SceneNode &operator=(SceneNode &&) = default;
    };
}

#endif
