#pragma once

#ifndef PIXELPULSE_IENTITY_H
#define PIXELPULSE_IENTITY_H

#include "../Platform/Std.h"
#include "Events/UpdateEventPayload.h"
#include "Events/AttachEventPayload.h"

namespace PixelPulse::Game
{
    class SceneNode;

    typedef const char* EntityID;

    class IEntity
    {
    public:
        virtual ~IEntity() = default;
        virtual void onAttach(SceneNode *ownerNode, const Events::AttachEventPayload &payload) = 0;
        virtual void onUpdate(SceneNode *ownerNode, const Events::UpdateEventPayload &payload) = 0;
        virtual void onDetach(SceneNode *ownerNode) = 0;
        virtual EntityID getID() const = 0;
    };
}

#endif
