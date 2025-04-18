#pragma once

#ifndef PIXELPULSE_FLOORENTITY_H
#define PIXELPULSE_FLOORENTITY_H

#include "../../Platform/Std.h"
#include "../../Game/IEntity.h"
#include "../../Assets/Image.h"
#include "../../Math/Vector2.h"
#include "../../Physics/Collider.h"
#include "../../Physics/RigidBody.h"
#include "../../Game/PhysicsComponent.h"

namespace PixelPulse::Entities
{
    class FloorEntity : public Game::IEntity
    {
    private:
        Game::PhysicsComponent* m_physicsComponent;
        Physics::BoxCollider* m_collider;
        Assets::Image* m_floorImage;
        int m_tileCount;
        float m_tileWidth;
        float m_floorWidth;
        float m_floorHeight;

    public:
        FloorEntity();
        virtual ~FloorEntity();

        void onAttach(Game::SceneNode *ownerNode, const Game::Events::AttachEventPayload &payload) override;
        void onStart(Game::SceneNode *ownerNode, const Game::Events::StartEventPayload &payload) override;
        void onDetach(Game::SceneNode *ownerNode) override;

        static Game::EntityID getID()
        {
            return "FloorEntity";
        }
    };
}

#endif
