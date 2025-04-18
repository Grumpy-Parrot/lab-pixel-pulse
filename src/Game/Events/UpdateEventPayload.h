#pragma once

#ifndef PIXELPULSE_UPDATEEVENTPAYLOAD_H
#define PIXELPULSE_UPDATEEVENTPAYLOAD_H

#include "../../Platform/Std.h"
#include "../Input.h"
#include "../../Physics/PhysicsWorld.h"

namespace PixelPulse::Game::Events
{
    struct UpdateEventPayload
    {
        float deltaTime;
        Input *input;
        Physics::PhysicsWorld *physicsWorld;
    };
}

#endif
