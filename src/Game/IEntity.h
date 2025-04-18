#pragma once

#ifndef PIXELPULSE_IENTITY_H
#define PIXELPULSE_IENTITY_H

#include "../Platform/Std.h"
#include "Events/UpdateEventPayload.h"
#include "Events/AttachEventPayload.h"
#include "Events/StartEventPayload.h"

namespace PixelPulse::Physics
{
    class Collider;
    struct CollisionInfo;
}

namespace PixelPulse::Game
{
    class SceneNode;

    typedef const char *EntityID;

    class IEntity
    {
    public:
        virtual ~IEntity() = default;
        virtual void onAttach(SceneNode *ownerNode, const Events::AttachEventPayload &payload)
        {
            PIXELPULSE_ARG_UNUSED(ownerNode);
            PIXELPULSE_ARG_UNUSED(payload);
        }

        virtual void onStart(SceneNode *ownerNode, const Events::StartEventPayload &payload)
        {
            PIXELPULSE_ARG_UNUSED(ownerNode);
            PIXELPULSE_ARG_UNUSED(payload);
        }

        virtual void onUpdate(SceneNode *ownerNode, const Events::UpdateEventPayload &payload)
        {
            PIXELPULSE_ARG_UNUSED(ownerNode);
            PIXELPULSE_ARG_UNUSED(payload);
        }

        virtual void onDetach(SceneNode *ownerNode)
        {
            PIXELPULSE_ARG_UNUSED(ownerNode);
        }

        virtual void onCollision(Physics::Collider *collider, const Physics::CollisionInfo &info)
        {

            PIXELPULSE_ARG_UNUSED(collider);
            PIXELPULSE_ARG_UNUSED(info);
        }

        static EntityID getID() { return "UnknownEntity"; }
    };
}

#endif
