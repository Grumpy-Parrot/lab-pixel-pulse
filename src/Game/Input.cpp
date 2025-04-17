#include "Input.h"
#include "../Logger.h"

using namespace PixelPulse::Game;
using namespace PixelPulse::Math;

Input::Input()
{
}

Input::~Input()
{
}

bool Input::isKeyPressed(KeyCode key)
{
    return SDL_GetKeyboardState(nullptr)[static_cast<int>(key)];
}

bool Input::isMouseButtonPressed(int button)
{
    return SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON_MASK(button);
}

Vector2<float> Input::getMousePosition()
{
    Vector2<float> mousePosition;
    SDL_GetMouseState(&mousePosition.x, &mousePosition.y);
    return mousePosition;
}

Vector2<float> Input::getMouseDelta()
{
    Vector2<float> mouseDelta;
    SDL_GetRelativeMouseState(&mouseDelta.x, &mouseDelta.y);
    return mouseDelta;
}

void Input::update(float deltaTime)
{
    (void)deltaTime; // Unused for now
    // Update logic
}
