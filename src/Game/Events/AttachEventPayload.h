#pragma once

#ifndef PIXELPULSE_ATTACHEVENTPAYLOAD_H
#define PIXELPULSE_ATTACHEVENTPAYLOAD_H

#include "Libraries/SDL.h"
#include "Platform/Std.h"
#include "../../Assets/AssetRegistry.h"
#include "../../Physics/PhysicsWorld.h"

namespace PixelPulse::Game::Events
{
    struct AttachEventPayload
    {
        Assets::AssetRegistry *assetRegistry;
        SDL_Renderer *renderer;
        Physics::PhysicsWorld *physicsWorld;
    };
}

#endif
