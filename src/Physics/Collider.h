#pragma once

#ifndef PIXELPULSE_COLLIDER_H
#define PIXELPULSE_COLLIDER_H

#include "../Math/Vector2.h"

namespace PixelPulse::Physics
{
    class RigidBody;
    class CollisionListener;
    class Collider;

    struct CollisionInfo
    {
        Math::Vector2<float> normal;
        float penetration;
        Collider *colliderA;
        Collider *colliderB;
    };

    enum class ColliderType
    {
        Box,
        Circle
    };

    class Collider
    {
    public:
        Collider(RigidBody *body);
        virtual ~Collider();

        virtual ColliderType getType() const = 0;

        void setOffset(const Math::Vector2<float> &offset);
        const Math::Vector2<float> &getOffset() const;

        RigidBody *getBody() const;

        void setListener(CollisionListener *listener);
        CollisionListener *getListener() const;

        Math::Vector2<float> getWorldPosition() const;

    protected:
        RigidBody *m_body;
        Math::Vector2<float> m_offset;
        CollisionListener *m_listener;
    };

    class BoxCollider : public Collider
    {
    public:
        BoxCollider(RigidBody *body, const Math::Vector2<float> &size);
        ~BoxCollider();

        ColliderType getType() const override;

        void setSize(const Math::Vector2<float> &size);
        const Math::Vector2<float> &getSize() const;

        Math::Vector2<float> getHalfSize() const;

    private:
        Math::Vector2<float> m_size;
    };

    class CircleCollider : public Collider
    {
    public:
        CircleCollider(RigidBody *body, float radius);
        ~CircleCollider();

        ColliderType getType() const override;

        void setRadius(float radius);
        float getRadius() const;

    private:
        float m_radius;
    };
}

#endif
