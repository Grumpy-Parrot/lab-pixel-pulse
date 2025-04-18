#include "Collider.h"
#include "RigidBody.h"

namespace PixelPulse::Physics
{
    Collider::Collider(RigidBody *body)
        : m_body(body), m_offset(0.0f, 0.0f), m_listener(nullptr)
    {
    }

    Collider::~Collider()
    {
    }

    void Collider::setOffset(const Math::Vector2<float> &offset)
    {
        m_offset = offset;
    }

    const Math::Vector2<float> &Collider::getOffset() const
    {
        return m_offset;
    }

    RigidBody *Collider::getBody() const
    {
        return m_body;
    }

    void Collider::setListener(CollisionListener *listener)
    {
        m_listener = listener;
    }

    CollisionListener *Collider::getListener() const
    {
        return m_listener;
    }

    Math::Vector2<float> Collider::getWorldPosition() const
    {
        return m_body->getPosition() + m_offset;
    }

    BoxCollider::BoxCollider(RigidBody *body, const Math::Vector2<float> &size)
        : Collider(body), m_size(size)
    {
    }

    BoxCollider::~BoxCollider()
    {
    }

    ColliderType BoxCollider::getType() const
    {
        return ColliderType::Box;
    }

    void BoxCollider::setSize(const Math::Vector2<float> &size)
    {
        m_size = size;
    }

    const Math::Vector2<float> &BoxCollider::getSize() const
    {
        return m_size;
    }

    Math::Vector2<float> BoxCollider::getHalfSize() const
    {
        return m_size * 0.5f;
    }

    CircleCollider::CircleCollider(RigidBody *body, float radius)
        : Collider(body), m_radius(radius)
    {
    }

    CircleCollider::~CircleCollider()
    {
    }

    ColliderType CircleCollider::getType() const
    {
        return ColliderType::Circle;
    }

    void CircleCollider::setRadius(float radius)
    {
        m_radius = radius;
    }

    float CircleCollider::getRadius() const
    {
        return m_radius;
    }
}
