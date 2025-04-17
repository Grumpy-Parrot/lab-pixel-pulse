#pragma once

#ifndef PIXELPULSE_SPRITE_H
#define PIXELPULSE_SPRITE_H

#include "../Libraries/Libraries.h"
#include "../Platform/Std.h"
#include "../Assets/Image.h"
#include "../Game/RenderPassDescriptor.h"
#include "../Math/Vector2.h"
#include "Events/UpdateEventPayload.h"

namespace PixelPulse::Game
{
    using namespace PixelPulse::Assets;
    using namespace PixelPulse::Game::Events;

    class SceneNode;

    class Sprite
    {
    private:
        Image *m_image;
        SDL_Texture *m_texture;
        Math::Vector2<std::int32_t> m_originalSize;

    public:
        Sprite(Image *image);
        virtual ~Sprite();

        bool init(SDL_Renderer *renderer);
        void onUpdate(const Events::UpdateEventPayload payload);
        void render(SDL_Renderer *renderer, const RenderPassDescriptor *renderPassDescriptor, const Math::Vector2<float> &worldPosition, const Math::Vector2<float> &worldScale);
        float getScalingFactor(int windowWidth, int windowHeight) const;
    };
}

#endif
