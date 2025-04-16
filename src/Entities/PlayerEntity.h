#pragma once

#ifndef PIXELPULSE_PLAYERENTITY_H
#define PIXELPULSE_PLAYERENTITY_H

#include "../Game/IEntity.h"
#include "../Math/Vector2.h"

namespace PixelPulse::Entities
{
    class PlayerEntity : public Game::IEntity
    {
    private:
        float m_moveSpeed;

    public:
        PlayerEntity();
        virtual ~PlayerEntity() = default;

        void onUpdate(Game::SceneNode *ownerNode, const Game::Events::UpdateEventPayload &payload) override;
        void onAttach(Game::SceneNode *ownerNode, const Game::Events::AttachEventPayload &payload) override;
        void onDetach(Game::SceneNode *ownerNode) override;
    };
}

#endif
