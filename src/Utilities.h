#pragma once

#ifndef PIXELPULSE_UTILITIES_H
#define PIXELPULSE_UTILITIES_H

#include "Platform/Std.h"

const char* PIXELPULSE_allocateId(size_t length = 10);

const char* PIXELPULSE_deriveId(const char* in, size_t length = 10);

void PIXELPULSE_freeId(const char* id);

#define PIXELPULSE_MAKE_ID() PIXELPULSE_allocateId(10)
#define PIXELPULSE_MAKE_ID_DERIVED(in) PIXELPULSE_deriveId(in, 10)
#define PIXELPULSE_FREE_ID(id)         \
    do {                       \
        if (id) {              \
            PIXELPULSE_freeId(id);     \
            id = nullptr;      \
        }                      \
    } while (0)
#define PIXELPULSE_FREE_ID_DERIVED(id) PIXELPULSE_FREE_ID(id)

#endif
