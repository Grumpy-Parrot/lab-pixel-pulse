#include "Sprite.h"
#include "../Logger.h"

Sprite::Sprite(Image *img) : image(img), texture(nullptr) {}

Sprite::~Sprite()
{
    if (texture)
    {
        SDL_DestroyTexture(texture);
    }
}

bool Sprite::init(SDL_Renderer *renderer)
{
    if (!image || !image->data)
    {
        Logger::error("Image data is null");
        return false;
    }

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC,
                                image->width, image->height);
    if (!texture)
    {
        Logger::error("Failed to create texture: %s", SDL_GetError());
        return false;
    }

    SDL_UpdateTexture(texture, nullptr, image->data, image->width * 4);
    return true;
}

void Sprite::render(SDL_Renderer *renderer, float x, float y)
{
    if (texture)
    {
        SDL_FRect dstRect = {x, y, static_cast<float>(image->width), static_cast<float>(image->height)};
        SDL_RenderTexture(renderer, texture, nullptr, &dstRect);
    }
}
