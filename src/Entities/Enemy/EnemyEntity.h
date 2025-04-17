#pragma once

#ifndef PIXELPULSE_ENEMYENTITY_H
#define PIXELPULSE_ENEMYENTITY_H

#include "../../Platform/Std.h"
#include "../../Game/IEntity.h"
#include "../../Math/Vector2.h"

namespace PixelPulse::Entities
{
    class EnemyEntity : public Game::IEntity
    {
    private:
        float m_moveSpeed;

    public:
        EnemyEntity();
        virtual ~EnemyEntity() = default;

        void onUpdate(Game::SceneNode *ownerNode, const Game::Events::UpdateEventPayload &payload) override;
        void onAttach(Game::SceneNode *ownerNode, const Game::Events::AttachEventPayload &payload) override;
        void onDetach(Game::SceneNode *ownerNode) override;

        static Game::EntityID getID()
        {
            return "EnemyEntity1";
        }
    };
}

#endif
