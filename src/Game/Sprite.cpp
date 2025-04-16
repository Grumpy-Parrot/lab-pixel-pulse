#include "Sprite.h"
#include "../Logger.h"

using namespace PixelPulse::Game;

Sprite::Sprite(Image *image)
    : m_image(image),
      m_texture(nullptr),
      m_originalSize(0, 0)
{
}

Sprite::~Sprite()
{
    if (m_texture)
    {
        SDL_DestroyTexture(m_texture);
        m_texture = nullptr;
    }

    if (m_image)
    {
        m_image->release();
        m_image = nullptr;
    }
}

bool Sprite::init(SDL_Renderer *renderer)
{
    if (!m_image || !m_image->data)
    {
        Logger::error("Image data is null");
        return false;
    }

    m_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC,
                                m_image->width, m_image->height);
    if (!m_texture)
    {
        Logger::error("Failed to create texture: %s", SDL_GetError());
        return false;
    }

    m_image->retain();

    m_originalSize.x = m_image->width;
    m_originalSize.y = m_image->height;

    SDL_UpdateTexture(m_texture, nullptr, m_image->data, m_image->width * 4);
    return true;
}

void Sprite::onUpdate(const UpdateEventPayload payload)
{
    (void)payload; // Unused for now
}

float Sprite::getScalingFactor(int windowWidth, int windowHeight) const
{
    // TODO: Remove hardcoded values
    // These should be replaced with actual window dimensions

    float widthRatio = static_cast<float>(windowWidth) / 1280.0f;
    float heightRatio = static_cast<float>(windowHeight) / 720.0f;
    return std::min(widthRatio, heightRatio);
}

void Sprite::render(SDL_Renderer *renderer, const RenderPassDescriptor *renderPassDescriptor, const Math::Vector2<float> &worldPosition, const Math::Vector2<float> &worldScale)
{
    if (m_texture)
    {
        float scaleFactor = getScalingFactor(renderPassDescriptor->windowSize.x, renderPassDescriptor->windowSize.y);

        // Use world scale passed in
        float scaledWidth = static_cast<float>(m_originalSize.x) * scaleFactor * worldScale.x;
        float scaledHeight = static_cast<float>(m_originalSize.y) * scaleFactor * worldScale.y;

        // Use world position passed in
        float scaledX = worldPosition.x * scaleFactor;
        float scaledY = worldPosition.y * scaleFactor;

        SDL_FRect dstRect;
        dstRect.x = scaledX;
        dstRect.y = scaledY;
        dstRect.w = scaledWidth;
        dstRect.h = scaledHeight;

        SDL_RenderTexture(renderer, m_texture, nullptr, &dstRect);
    }
}
