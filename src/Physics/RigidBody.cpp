#include "RigidBody.h"
#include "Collider.h"
#include "PhysicsWorld.h"

namespace PixelPulse::Physics
{
    RigidBody::RigidBody(PhysicsWorld *world, const Math::Vector2<float> &position)
        : m_world(world), m_position(position), m_velocity(0.0f, 0.0f), m_force(0.0f, 0.0f), m_rotation(0.0f), m_angularVelocity(0.0f), m_torque(0.0f), m_mass(1.0f), m_inverseMass(1.0f), m_inertia(0.0f), m_inverseInertia(0.0f), m_restitution(0.2f), m_friction(0.1f), m_isStatic(false)
    {
    }

    RigidBody::~RigidBody()
    {
        for (auto collider : m_colliders)
        {
            m_world->removeCollider(collider);
        }

        m_colliders.clear();
    }

    void RigidBody::applyForce(const Math::Vector2<float> &force)
    {
        if (!m_isStatic)
        {
            m_force += force;
        }
    }

    void RigidBody::applyImpulse(const Math::Vector2<float> &impulse)
    {
        if (!m_isStatic)
        {
            m_velocity += impulse * m_inverseMass;
        }
    }

    void RigidBody::setPosition(const Math::Vector2<float> &position)
    {
        m_position = position;
    }

    const Math::Vector2<float> &RigidBody::getPosition() const
    {
        return m_position;
    }

    void RigidBody::setVelocity(const Math::Vector2<float> &velocity)
    {
        m_velocity = velocity;
    }

    const Math::Vector2<float> &RigidBody::getVelocity() const
    {
        return m_velocity;
    }

    void RigidBody::setRotation(float rotation)
    {
        m_rotation = rotation;
    }

    float RigidBody::getRotation() const
    {
        return m_rotation;
    }

    void RigidBody::setAngularVelocity(float angularVelocity)
    {
        m_angularVelocity = angularVelocity;
    }

    float RigidBody::getAngularVelocity() const
    {
        return m_angularVelocity;
    }

    void RigidBody::setMass(float mass)
    {
        m_mass = mass;
        m_inverseMass = mass > 0.0f ? 1.0f / mass : 0.0f;

        updateInertia();
    }

    float RigidBody::getMass() const
    {
        return m_mass;
    }

    float RigidBody::getInverseMass() const
    {
        return m_inverseMass;
    }

    void RigidBody::setRestitution(float restitution)
    {
        m_restitution = restitution;
    }

    float RigidBody::getRestitution() const
    {
        return m_restitution;
    }

    void RigidBody::setFriction(float friction)
    {
        m_friction = friction;
    }

    float RigidBody::getFriction() const
    {
        return m_friction;
    }

    void RigidBody::setStatic(bool isStatic)
    {
        m_isStatic = isStatic;

        if (m_isStatic)
        {
            m_velocity = Math::Vector2<float>(0.0f, 0.0f);
            m_angularVelocity = 0.0f;
            m_inverseMass = 0.0f;
            m_inverseInertia = 0.0f;
        }
        else
        {
            m_inverseMass = m_mass > 0.0f ? 1.0f / m_mass : 0.0f;
            updateInertia();
        }
    }

    bool RigidBody::isStatic() const
    {
        return m_isStatic;
    }

    void RigidBody::addCollider(Collider *collider)
    {
        if (std::find(m_colliders.begin(), m_colliders.end(), collider) == m_colliders.end())
        {
            m_colliders.push_back(collider);
            updateInertia();
        }
    }

    void RigidBody::removeCollider(Collider *collider)
    {
        auto it = std::find(m_colliders.begin(), m_colliders.end(), collider);
        if (it != m_colliders.end())
        {
            m_colliders.erase(it);
            updateInertia();
        }
    }

    void RigidBody::integrate(float deltaTime)
    {
        if (m_isStatic)
            return;

        m_velocity += m_force * m_inverseMass * deltaTime;
        m_position += m_velocity * deltaTime;

        m_angularVelocity += m_torque * m_inverseInertia * deltaTime;
        m_rotation += m_angularVelocity * deltaTime;

        m_force = Math::Vector2<float>(0.0f, 0.0f);
        m_torque = 0.0f;
    }

    void RigidBody::updateInertia()
    {
        if (m_isStatic)
        {
            m_inertia = 0.0f;
            m_inverseInertia = 0.0f;
            return;
        }

        m_inertia = 0.0f;

        for (auto collider : m_colliders)
        {
            if (collider->getType() == ColliderType::Circle)
            {
                CircleCollider *circle = static_cast<CircleCollider *>(collider);
                float radius = circle->getRadius();
                m_inertia += 0.5f * m_mass * radius * radius;
            }
            else if (collider->getType() == ColliderType::Box)
            {
                BoxCollider *box = static_cast<BoxCollider *>(collider);
                const Math::Vector2<float> &size = box->getSize();
                m_inertia += m_mass * (size.x * size.x + size.y * size.y) / 12.0f;
            }
        }

        m_inverseInertia = m_inertia > 0.0f ? 1.0f / m_inertia : 0.0f;
    }
}
