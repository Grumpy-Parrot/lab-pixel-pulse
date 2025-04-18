#pragma once

#ifndef PIXELPULSE_ENEMYENTITY_H
#define PIXELPULSE_ENEMYENTITY_H

#include "../../Platform/Std.h"
#include "../../Game/IEntity.h"
#include "../../Assets/Image.h"
#include "../../Math/Vector2.h"
#include "../../Game/PhysicsComponent.h"

namespace PixelPulse::Entities
{
    class EnemyEntity : public Game::IEntity
    {
    private:
        float m_moveSpeed;
        Game::PhysicsComponent* m_physicsComponent;
        PixelPulse::Physics::BoxCollider* m_collider;
        Assets::Image* m_enemyImage;

    public:
        EnemyEntity();
        virtual ~EnemyEntity();

        void onUpdate(Game::SceneNode *ownerNode, const Game::Events::UpdateEventPayload &payload) override;
        void onAttach(Game::SceneNode *ownerNode, const Game::Events::AttachEventPayload &payload) override;
        void onStart(Game::SceneNode *ownerNode, const Game::Events::StartEventPayload &payload) override;
        void onDetach(Game::SceneNode *ownerNode) override;
        void onCollision(PixelPulse::Physics::Collider *collider, const PixelPulse::Physics::CollisionInfo &info) override;

        static Game::EntityID getID()
        {
            return "EnemyEntity1";
        }
    };
}

#endif
