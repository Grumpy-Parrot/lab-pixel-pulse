#pragma once

#ifndef PLATFORM_API_H
#define PLATFORM_API_H

#ifdef PLATFORM_WASM
#include <emscripten/emscripten.h>
#endif

#ifdef PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define WIN32_EXTRA_LEAN
#include <windows.h>
#endif

#ifdef PLATFORM_MACOS
#include <TargetConditionals.h>

#if defined(__OBJC__) || defined(__OBJC2__)
#include <CoreFoundation/CoreFoundation.h>
#include <Cocoa/Cocoa.h>
#endif

#endif

#endif
