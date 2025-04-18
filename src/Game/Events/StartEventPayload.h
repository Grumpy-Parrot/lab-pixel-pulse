#pragma once

#ifndef PIXELPULSE_STARTEVENTPAYLOAD_H
#define PIXELPULSE_STARTEVENTPAYLOAD_H

#include "Libraries/SDL.h"
#include "Platform/Std.h"
#include "../../Assets/AssetRegistry.h"
#include "../../Physics/PhysicsWorld.h"

namespace PixelPulse::Game::Events
{
    struct StartEventPayload
    {
        Assets::AssetRegistry *assetRegistry;
        SDL_Renderer *renderer;
        Physics::PhysicsWorld *physicsWorld;
    };
}

#endif
