#pragma once

#ifndef PIXELPULSE_RIGIDBODY_H
#define PIXELPULSE_RIGIDBODY_H

#include "../Math/Vector2.h"

namespace PixelPulse::Physics
{
    class PhysicsWorld;
    class Collider;

    class RigidBody
    {
    public:
        RigidBody(PhysicsWorld *world, const Math::Vector2<float> &position);
        ~RigidBody();

        void applyForce(const Math::Vector2<float> &force);
        void applyImpulse(const Math::Vector2<float> &impulse);

        void setPosition(const Math::Vector2<float> &position);
        const Math::Vector2<float> &getPosition() const;

        void setVelocity(const Math::Vector2<float> &velocity);
        const Math::Vector2<float> &getVelocity() const;

        void setRotation(float rotation);
        float getRotation() const;

        void setAngularVelocity(float angularVelocity);
        float getAngularVelocity() const;

        void setMass(float mass);
        float getMass() const;
        float getInverseMass() const;

        void setRestitution(float restitution);
        float getRestitution() const;

        void setFriction(float friction);
        float getFriction() const;

        void setStatic(bool isStatic);
        bool isStatic() const;

        void addCollider(Collider *collider);
        void removeCollider(Collider *collider);

        void integrate(float deltaTime);

        void updateInertia();

    private:
        PhysicsWorld *m_world;
        Math::Vector2<float> m_position;
        Math::Vector2<float> m_velocity;
        Math::Vector2<float> m_force;

        float m_rotation;
        float m_angularVelocity;
        float m_torque;

        float m_mass;
        float m_inverseMass;
        float m_inertia;
        float m_inverseInertia;

        float m_restitution;
        float m_friction;

        bool m_isStatic;

        std::vector<Collider *> m_colliders;

        friend class PhysicsWorld;
    };
}

#endif
