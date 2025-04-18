#include "PhysicsComponent.h"
#include "IEntity.h"
#include "../Physics/Collider.h"
#include "../Logger.h"

namespace PixelPulse::Game
{
    PhysicsComponent::PhysicsComponent(IEntity* owner)
        : m_owner(owner)
        , m_rigidBody(nullptr)
        , m_physicsWorld(nullptr)
    {
    }

    PhysicsComponent::~PhysicsComponent()
    {
        for (auto* collider : m_colliders)
        {
            if (m_physicsWorld)
            {
                m_physicsWorld->removeCollider(collider);
            }
        }
        m_colliders.clear();

        if (m_rigidBody && m_physicsWorld)
        {
            m_physicsWorld->removeRigidBody(m_rigidBody);
            m_rigidBody = nullptr;
        }
    }

    void PhysicsComponent::initialize(Physics::PhysicsWorld* physicsWorld, const Math::Vector2<float>& position)
    {
        if (!physicsWorld)
        {
            Logger::error("PhysicsComponent::initialize called with null physics world");
            return;
        }

        m_physicsWorld = physicsWorld;
        m_rigidBody = m_physicsWorld->createRigidBody(position);
    }

    Physics::BoxCollider* PhysicsComponent::createBoxCollider(const Math::Vector2<float>& size, const Math::Vector2<float>& offset)
    {
        if (!m_rigidBody || !m_physicsWorld)
        {
            Logger::error("PhysicsComponent::createBoxCollider called before initialization");
            return nullptr;
        }

        Physics::BoxCollider* collider = m_physicsWorld->createBoxCollider(m_rigidBody, size);
        if (collider)
        {
            collider->setOffset(offset);
            collider->setListener(this);
            m_colliders.push_back(collider);
        }
        return collider;
    }

    Physics::CircleCollider* PhysicsComponent::createCircleCollider(float radius, const Math::Vector2<float>& offset)
    {
        if (!m_rigidBody || !m_physicsWorld)
        {
            Logger::error("PhysicsComponent::createCircleCollider called before initialization");
            return nullptr;
        }

        Physics::CircleCollider* collider = m_physicsWorld->createCircleCollider(m_rigidBody, radius);
        if (collider)
        {
            collider->setOffset(offset);
            collider->setListener(this);
            m_colliders.push_back(collider);
        }
        return collider;
    }

    void PhysicsComponent::updatePosition(const Math::Vector2<float>& position)
    {
        if (m_rigidBody)
        {
            m_rigidBody->setPosition(position);
        }
    }

    Math::Vector2<float> PhysicsComponent::getPosition() const
    {
        if (m_rigidBody)
        {
            return m_rigidBody->getPosition();
        }
        return Math::Vector2<float>(0.0f, 0.0f);
    }

    void PhysicsComponent::setVelocity(const Math::Vector2<float>& velocity)
    {
        if (m_rigidBody)
        {
            m_rigidBody->setVelocity(velocity);
        }
    }

    void PhysicsComponent::applyForce(const Math::Vector2<float>& force)
    {
        if (m_rigidBody)
        {
            m_rigidBody->applyForce(force);
        }
    }

    void PhysicsComponent::applyImpulse(const Math::Vector2<float>& impulse)
    {
        if (m_rigidBody)
        {
            m_rigidBody->applyImpulse(impulse);
        }
    }

    void PhysicsComponent::setStatic(bool isStatic)
    {
        if (m_rigidBody)
        {
            m_rigidBody->setStatic(isStatic);
        }
    }

    void PhysicsComponent::onCollisionEnter(Physics::Collider* self, Physics::Collider* other, const Physics::CollisionInfo& info)
    {
        PIXELPULSE_ARG_UNUSED(self);

        if (m_owner)
        {
            m_owner->onCollision(other, info);
        }
    }

    void PhysicsComponent::onCollisionStay(Physics::Collider* self, Physics::Collider* other, const Physics::CollisionInfo& info)
    {
        PIXELPULSE_ARG_UNUSED(self);

        if (m_owner)
        {
            m_owner->onCollision(other, info);
        }
    }

    void PhysicsComponent::onCollisionExit(Physics::Collider* self, Physics::Collider* other)
    {
        PIXELPULSE_ARG_UNUSED(self);
        PIXELPULSE_ARG_UNUSED(other);
    }
}
