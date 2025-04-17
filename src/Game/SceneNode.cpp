
#include "SceneNode.h"
#include "Sprite.h"
#include "../Platform/Platform.h"
#include "../Utilities.h"

using namespace PixelPulse::Game;

SceneNode::SceneNode() : m_position(0.0f, 0.0f),
                         m_scale(1.0f, 1.0f),
                         m_rotation(0.0f),
                         m_sprite(nullptr),
                         m_entity(nullptr),
                         m_parent(nullptr),
                         m_tag(nullptr)
{
}

SceneNode::~SceneNode()
{
    if (m_entity)
    {
        m_entity->onDetach(this);
        m_entity = nullptr;
    }

    for (SceneNode *child : m_children)
    {
        PP_DELETE(child);
    }
    m_children.clear();

    PIXELPULSE_FREE_ID(m_tag);
}

void SceneNode::setTag(const char *tag)
{
    PIXELPULSE_FREE_ID(m_tag);

    m_tag = tag;
}

void SceneNode::setSprite(Sprite *sprite)
{
    m_sprite = sprite;
}

void SceneNode::setEntity(IEntity *entity)
{
    m_entity = entity;
}

void SceneNode::update(const UpdateEventPayload &payload, const Math::Vector2<float> &parentPosition, const Math::Vector2<float> &parentScale)
{
    Math::Vector2<float> globalPosition = parentPosition + m_position;
    Math::Vector2<float> globalScale = parentScale * m_scale;

    if (m_entity)
    {
        m_entity->onUpdate(this, payload);
    }

    for (SceneNode *child : m_children)
    {
        child->update(payload, globalPosition, globalScale);
    }
}

void SceneNode::render(::SDL_Renderer *renderer, const RenderPassDescriptor *renderPassDescriptor)
{
    if (m_sprite)
    {
        m_sprite->render(renderer, renderPassDescriptor, m_position, m_scale);
    }

    for (const auto &child : m_children)
    {
        child->render(renderer, renderPassDescriptor);
    }
}

void SceneNode::addChild(SceneNode *child)
{
    if (child)
    {
        child->m_parent = this;
        m_children.push_back(child);
    }
}
