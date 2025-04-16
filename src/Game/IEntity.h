#pragma once

#ifndef PIXELPULSE_IENTITY_H
#define PIXELPULSE_IENTITY_H

#include "Events/UpdateEventPayload.h"
#include "Events/AttachEventPayload.h"

namespace PixelPulse::Game
{
    class SceneNode;

    class IEntity
    {
    public:
        virtual ~IEntity() = default;
        virtual void onAttach(SceneNode *ownerNode, const Events::AttachEventPayload &payload) = 0;
        virtual void onUpdate(SceneNode *ownerNode, const Events::UpdateEventPayload &payload) = 0;
        virtual void onDetach(SceneNode *ownerNode) = 0;
    };
}

#endif
