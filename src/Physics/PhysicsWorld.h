#pragma once

#ifndef PIXELPULSE_PHYSICS_WORLD_H
#define PIXELPULSE_PHYSICS_WORLD_H

#include "../Math/Vector2.h"
#include "Collider.h"
#include "RigidBody.h"
#include <vector>

namespace PixelPulse::Physics
{
    class PhysicsWorld
    {
    public:
        PhysicsWorld();
        ~PhysicsWorld();

        void update(float deltaTime);
        void step(float timeStep);

        RigidBody *createRigidBody(const Math::Vector2<float> &position);
        BoxCollider *createBoxCollider(RigidBody *body, const Math::Vector2<float> &size);
        CircleCollider *createCircleCollider(RigidBody *body, float radius);

        void setGravity(const Math::Vector2<float> &gravity);
        const Math::Vector2<float> &getGravity() const;

        void removeRigidBody(RigidBody *body);
        void removeCollider(Collider *collider);

    private:
        void resolveCollisions();
        void resolveCollision(const CollisionInfo &info);
        bool checkCollision(Collider *a, Collider *b, CollisionInfo &info);
        bool checkBoxBox(BoxCollider *a, BoxCollider *b, CollisionInfo &info);
        bool checkCircleCircle(CircleCollider *a, CircleCollider *b, CollisionInfo &info);
        bool checkBoxCircle(BoxCollider *a, CircleCollider *b, CollisionInfo &info);

        std::vector<RigidBody *> m_bodies;
        std::vector<Collider *> m_colliders;
        Math::Vector2<float> m_gravity;
    };
}

#endif
