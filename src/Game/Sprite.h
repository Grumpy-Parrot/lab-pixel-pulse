#pragma once

#ifndef GP_SPRITE_H
#define GP_SPRITE_H

#include "../Libraries/Libraries.h"
#include "../Assets/Image.h"

class Sprite
{
public:
    Image *image;
    SDL_Texture *texture;

    Sprite(Image *img);
    ~Sprite();

    bool init(SDL_Renderer *renderer);
    void render(SDL_Renderer *renderer, float x, float y);
};

#endif
