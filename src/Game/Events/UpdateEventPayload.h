#pragma once

#ifndef PIXELPULSE_UPDATEEVENTPAYLOAD_H
#define PIXELPULSE_UPDATEEVENTPAYLOAD_H

#include "../../Platform/Std.h"
#include "../Input.h"

namespace PixelPulse::Game::Events
{
    struct UpdateEventPayload
    {
        float deltaTime;
        Input *input;
    };
}

#endif
