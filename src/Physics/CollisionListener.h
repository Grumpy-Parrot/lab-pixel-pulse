#pragma once

#ifndef PIXELPULSE_COLLISION_LISTENER_H
#define PIXELPULSE_COLLISION_LISTENER_H

#include "Collider.h"

namespace PixelPulse::Physics
{
    class CollisionListener
    {
    public:
        virtual ~CollisionListener() {}

        virtual void onCollisionEnter(PixelPulse::Physics::Collider *self, PixelPulse::Physics::Collider *other, const CollisionInfo &info) = 0;

        virtual void onCollisionStay(PixelPulse::Physics::Collider *self, PixelPulse::Physics::Collider *other, const CollisionInfo &info) = 0;

        virtual void onCollisionExit(PixelPulse::Physics::Collider *self, PixelPulse::Physics::Collider *other) = 0;
    };
}

#endif
