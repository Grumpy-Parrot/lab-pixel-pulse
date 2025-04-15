#include "Sprite.h"
#include "../Logger.h"

Sprite::Sprite(Image *img) : image(img),
                             texture(nullptr),
                             m_originalSize(0, 0),
                             m_position(0, 0) {}

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

    m_originalSize.x = image->width;
    m_originalSize.y = image->height;

    SDL_UpdateTexture(texture, nullptr, image->data, image->width * 4);
    return true;
}

void Sprite::onUpdate(UpdateEventPayload payload)
{
    float deltaTime = payload.deltaTime;
    Input *input = payload.input;

    if (input->isKeyPressed(KeyCode::KEY_W))
    {
        m_position.y -= 100.0f * deltaTime;
    }

    if (input->isKeyPressed(KeyCode::KEY_S))
    {
        m_position.y += 100.0f * deltaTime;
    }

    if (input->isKeyPressed(KeyCode::KEY_A))
    {
        m_position.x -= 100.0f * deltaTime;
    }

    if (input->isKeyPressed(KeyCode::KEY_D))
    {
        m_position.x += 100.0f * deltaTime;
    }
}

void Sprite::move(Vector2<float> to)
{
    m_position = to;
}

void Sprite::scale(Vector2<float> factor)
{
    m_scale = factor;
}

float Sprite::getScalingFactor(int windowWidth, int windowHeight) const
{
    // TODO: Remove hardcoded values
    // These should be replaced with actual window dimensions

    float widthRatio = static_cast<float>(windowWidth) / 1280.0f;
    float heightRatio = static_cast<float>(windowHeight) / 720.0f;
    return std::min(widthRatio, heightRatio);
}

void Sprite::render(SDL_Renderer *renderer, RenderPassDescriptor *renderPassDescriptor)
{
    if (texture)
    {
        float scaleFactor = getScalingFactor(renderPassDescriptor->windowSize.x, renderPassDescriptor->windowSize.y);

        float scaledWidth = static_cast<float>(m_originalSize.x) * scaleFactor;
        float scaledHeight = static_cast<float>(m_originalSize.y) * scaleFactor;

        float scaledX = m_position.x * scaleFactor;
        float scaledY = m_position.y * scaleFactor;

        SDL_FRect dstRect;
        dstRect.x = scaledX;
        dstRect.y = scaledY;
        dstRect.w = scaledWidth * m_scale.x;
        dstRect.h = scaledHeight * m_scale.y;

        SDL_RenderTexture(renderer, texture, nullptr, &dstRect);
    }
}
