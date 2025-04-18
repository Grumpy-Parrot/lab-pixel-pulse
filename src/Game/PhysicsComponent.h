#pragma once

#ifndef PIXELPULSE_PHYSICS_COMPONENT_H
#define PIXELPULSE_PHYSICS_COMPONENT_H

#include "../Math/Vector2.h"
#include "../Physics/Collider.h"
#include "../Physics/RigidBody.h"
#include "../Physics/PhysicsWorld.h"
#include "../Physics/CollisionListener.h"

namespace PixelPulse::Game
{
    class SceneNode;
    class IEntity;

    class PhysicsComponent : public Physics::CollisionListener
    {
    public:
        PhysicsComponent(IEntity* owner);
        virtual ~PhysicsComponent();

        void initialize(Physics::PhysicsWorld* physicsWorld, const Math::Vector2<float>& position);

        Physics::BoxCollider* createBoxCollider(const Math::Vector2<float>& size, const Math::Vector2<float>& offset = Math::Vector2<float>(0.0f, 0.0f));

        Physics::CircleCollider* createCircleCollider(float radius, const Math::Vector2<float>& offset = Math::Vector2<float>(0.0f, 0.0f));

        void updatePosition(const Math::Vector2<float>& position);

        Math::Vector2<float> getPosition() const;

        void setVelocity(const Math::Vector2<float>& velocity);

        void applyForce(const Math::Vector2<float>& force);

        void applyImpulse(const Math::Vector2<float>& impulse);

        void setStatic(bool isStatic);

        Physics::RigidBody* getRigidBody() const { return m_rigidBody; }

        virtual void onCollisionEnter(Physics::Collider* self, Physics::Collider* other, const Physics::CollisionInfo& info) override;
        virtual void onCollisionStay(Physics::Collider* self, Physics::Collider* other, const Physics::CollisionInfo& info) override;
        virtual void onCollisionExit(Physics::Collider* self, Physics::Collider* other) override;

    private:
        IEntity* m_owner;
        Physics::RigidBody* m_rigidBody;
        Physics::PhysicsWorld* m_physicsWorld;
        std::vector<Physics::Collider*> m_colliders;
    };
}

#endif
