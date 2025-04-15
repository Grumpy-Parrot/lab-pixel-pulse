#pragma once

#ifndef GP_UPDATEEVENTPAYLOAD_H
#define GP_UPDATEEVENTPAYLOAD_H

#include "Platform/Std.h"
#include "Math/Vector2.h"
#include "Game/Input.h"

struct UpdateEventPayload
{
    float deltaTime;
    Input *input;
};

#endif
