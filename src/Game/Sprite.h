#pragma once

#ifndef GP_SPRITE_H
#define GP_SPRITE_H

#include "../Libraries/Libraries.h"
#include "../Assets/Image.h"
#include "../RenderPassDescriptor.h"
#include "../Math/Vector2.h"
#include "UpdateEventPayload.h"

class Sprite
{
private:
    Image *image;
    SDL_Texture *texture;

    Vector2<std::int32_t> m_originalSize;
    Vector2<float> m_position;
    Vector2<float> m_scale;

public:
    Sprite(Image *img);
    ~Sprite();

    bool init(SDL_Renderer *renderer);

    void onUpdate(UpdateEventPayload payload);

    void move(Vector2<float> to);

    void scale(Vector2<float> factor);

    void render(SDL_Renderer *renderer, RenderPassDescriptor *renderPassDescriptor);

    float getScalingFactor(int windowWidth, int windowHeight) const;
};

#endif
