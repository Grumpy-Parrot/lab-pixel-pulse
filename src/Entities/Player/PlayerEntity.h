#pragma once

#ifndef PIXELPULSE_PLAYERENTITY_H
#define PIXELPULSE_PLAYERENTITY_H

#include "../../Platform/Std.h"
#include "../../Game/IEntity.h"
#include "../../Assets/Image.h"
#include "../../Math/Vector2.h"

namespace PixelPulse::Entities
{
    class PlayerEntity : public Game::IEntity
    {
    private:
        float m_moveSpeed;
        Assets::Image *m_playerImage;

    public:
        PlayerEntity();
        virtual ~PlayerEntity() = default;

        void onUpdate(Game::SceneNode *ownerNode, const Game::Events::UpdateEventPayload &payload) override;
        void onAttach(Game::SceneNode *ownerNode, const Game::Events::AttachEventPayload &payload) override;
        void onDetach(Game::SceneNode *ownerNode) override;

        static Game::EntityID getID()
        {
            return "PlayerEntity";
        }
    };
}

#endif
