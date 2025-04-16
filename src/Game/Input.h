#pragma once

#ifndef PIXELPULSE_INPUT_H
#define PIXELPULSE_INPUT_H

#include "../Libraries/Libraries.h"
#include "../Math/Vector2.h"

namespace PixelPulse::Game
{
    enum class KeyCode
    {
        KEY_A = SDL_SCANCODE_A,
        KEY_B = SDL_SCANCODE_B,
        KEY_C = SDL_SCANCODE_C,
        KEY_D = SDL_SCANCODE_D,
        KEY_E = SDL_SCANCODE_E,
        KEY_F = SDL_SCANCODE_F,
        KEY_G = SDL_SCANCODE_G,
        KEY_H = SDL_SCANCODE_H,
        KEY_I = SDL_SCANCODE_I,
        KEY_J = SDL_SCANCODE_J,
        KEY_K = SDL_SCANCODE_K,
        KEY_L = SDL_SCANCODE_L,
        KEY_M = SDL_SCANCODE_M,
        KEY_N = SDL_SCANCODE_N,
        KEY_O = SDL_SCANCODE_O,
        KEY_P = SDL_SCANCODE_P,
        KEY_Q = SDL_SCANCODE_Q,
        KEY_R = SDL_SCANCODE_R,
        KEY_S = SDL_SCANCODE_S,
        KEY_T = SDL_SCANCODE_T,
        KEY_U = SDL_SCANCODE_U,
        KEY_V = SDL_SCANCODE_V,
        KEY_W = SDL_SCANCODE_W,
        KEY_X = SDL_SCANCODE_X,
        KEY_Y = SDL_SCANCODE_Y,
        KEY_Z = SDL_SCANCODE_Z,
        KEY_ESCAPE = SDL_SCANCODE_ESCAPE,
        KEY_SPACE = SDL_SCANCODE_SPACE,
        KEY_ENTER = SDL_SCANCODE_RETURN,
        KEY_BACKSPACE = SDL_SCANCODE_BACKSPACE,
    };

    class Input
    {
    public:
        Input();
        ~Input();

        bool isKeyPressed(KeyCode key);
        bool isMouseButtonPressed(int button);
        PixelPulse::Math::Vector2<float> getMousePosition();
        PixelPulse::Math::Vector2<float> getMouseDelta();
        void update(float deltaTime);
    };
}

#endif
