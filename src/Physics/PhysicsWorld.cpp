#include "PhysicsWorld.h"
#include "Collider.h"
#include "RigidBody.h"
#include "CollisionListener.h"
#include <algorithm>
#include <cmath>

namespace PixelPulse::Physics
{
    PhysicsWorld::PhysicsWorld()
        : m_gravity(0.0f, 9.8f)
    {
    }

    PhysicsWorld::~PhysicsWorld()
    {
        for (auto body : m_bodies)
        {
            delete body;
        }

        for (auto collider : m_colliders)
        {
            delete collider;
        }

        m_bodies.clear();
        m_colliders.clear();
    }

    void PhysicsWorld::update(float deltaTime)
    {
        step(deltaTime);
    }

    void PhysicsWorld::step(float timeStep)
    {
        for (auto body : m_bodies)
        {
            if (!body->isStatic())
            {
                body->applyForce(m_gravity * body->getMass());
                body->integrate(timeStep);
            }
        }

        resolveCollisions();
    }

    RigidBody *PhysicsWorld::createRigidBody(const Math::Vector2<float> &position)
    {
        RigidBody *body = PP_NEW(RigidBody, this, position);
        m_bodies.push_back(body);
        return body;
    }

    BoxCollider *PhysicsWorld::createBoxCollider(RigidBody *body, const Math::Vector2<float> &size)
    {
        BoxCollider *collider = PP_NEW(BoxCollider, body, size);
        m_colliders.push_back(collider);
        body->addCollider(collider);
        return collider;
    }

    CircleCollider *PhysicsWorld::createCircleCollider(RigidBody *body, float radius)
    {
        CircleCollider *collider = PP_NEW(CircleCollider, body, radius);
        m_colliders.push_back(collider);
        body->addCollider(collider);
        return collider;
    }

    void PhysicsWorld::setGravity(const Math::Vector2<float> &gravity)
    {
        m_gravity = gravity;
    }

    const Math::Vector2<float> &PhysicsWorld::getGravity() const
    {
        return m_gravity;
    }

    void PhysicsWorld::removeRigidBody(RigidBody *body)
    {
        auto it = std::find(m_bodies.begin(), m_bodies.end(), body);
        if (it != m_bodies.end())
        {
            for (auto collider : body->m_colliders)
            {
                removeCollider(collider);
            }

            m_bodies.erase(it);
            PP_DELETE(body);
        }
    }

    void PhysicsWorld::removeCollider(Collider *collider)
    {
        auto it = std::find(m_colliders.begin(), m_colliders.end(), collider);
        if (it != m_colliders.end())
        {
            m_colliders.erase(it);

            RigidBody *body = collider->getBody();
            if (body)
            {
                body->removeCollider(collider);
            }

            PP_DELETE(collider);
        }
    }

    void PhysicsWorld::resolveCollisions()
    {
        static std::vector<std::pair<Collider *, Collider *>> currentCollisions;
        static std::vector<std::pair<Collider *, Collider *>> lastFrameCollisions;

        std::swap(currentCollisions, lastFrameCollisions);
        currentCollisions.clear();

        for (size_t i = 0; i < m_colliders.size(); i++)
        {
            for (size_t j = i + 1; j < m_colliders.size(); j++)
            {
                Collider *a = m_colliders[i];
                Collider *b = m_colliders[j];

                RigidBody *bodyA = a->getBody();
                RigidBody *bodyB = b->getBody();

                if (bodyA == bodyB)
                    continue;

                if (bodyA->isStatic() && bodyB->isStatic())
                    continue;

                CollisionInfo info;
                if (checkCollision(a, b, info))
                {
                    currentCollisions.push_back(std::make_pair(a, b));

                    CollisionListener *listenerA = a->getListener();
                    CollisionListener *listenerB = b->getListener();

                    bool wasColliding = false;
                    for (const auto &pair : lastFrameCollisions)
                    {
                        if ((pair.first == a && pair.second == b) ||
                            (pair.first == b && pair.second == a))
                        {
                            wasColliding = true;
                            break;
                        }
                    }

                    Math::Vector2<float> relativeVelocity = bodyB->getVelocity() - bodyA->getVelocity();
                    float velocityMagnitudeSquared = relativeVelocity.lengthSquared();
                    bool bodiesAtRest = velocityMagnitudeSquared < 0.001f;

                    if (!wasColliding)
                    {
                        if (listenerA)
                            listenerA->onCollisionEnter(a, b, info);
                        if (listenerB)
                        {
                            CollisionInfo reversedInfo = info;
                            reversedInfo.normal = info.normal * -1.0f;
                            std::swap(reversedInfo.colliderA, reversedInfo.colliderB);
                            listenerB->onCollisionEnter(b, a, reversedInfo);
                        }
                    }
                    else if (!bodiesAtRest)
                    {
                        if (listenerA)
                            listenerA->onCollisionStay(a, b, info);
                        if (listenerB)
                        {
                            CollisionInfo reversedInfo = info;
                            reversedInfo.normal = info.normal * -1.0f;
                            std::swap(reversedInfo.colliderA, reversedInfo.colliderB);
                            listenerB->onCollisionStay(b, a, reversedInfo);
                        }
                    }

                    resolveCollision(info);
                }
            }
        }

        for (const auto &pair : lastFrameCollisions)
        {
            bool stillColliding = false;
            for (const auto &currentPair : currentCollisions)
            {
                if ((currentPair.first == pair.first && currentPair.second == pair.second) ||
                    (currentPair.first == pair.second && currentPair.second == pair.first))
                {
                    stillColliding = true;
                    break;
                }
            }

            if (!stillColliding)
            {
                Collider *a = pair.first;
                Collider *b = pair.second;

                CollisionListener *listenerA = a->getListener();
                CollisionListener *listenerB = b->getListener();

                if (listenerA)
                    listenerA->onCollisionExit(a, b);
                if (listenerB)
                    listenerB->onCollisionExit(b, a);
            }
        }
    }

    void PhysicsWorld::resolveCollision(const CollisionInfo &info)
    {
        RigidBody *bodyA = info.colliderA->getBody();
        RigidBody *bodyB = info.colliderB->getBody();

        Math::Vector2<float> relativeVelocity = bodyB->getVelocity() - bodyA->getVelocity();

        float velocityAlongNormal = relativeVelocity.x * info.normal.x + relativeVelocity.y * info.normal.y;

        if (velocityAlongNormal > 0)
            return;

        float restitution = std::min(bodyA->getRestitution(), bodyB->getRestitution());

        float j = -(1.0f + restitution) * velocityAlongNormal;
        j /= bodyA->getInverseMass() + bodyB->getInverseMass();

        Math::Vector2<float> impulse = info.normal * j;

        bodyA->applyImpulse(impulse * -1.0f);
        bodyB->applyImpulse(impulse);

        const float percent = 0.8f;
        const float slop = 0.001f;
        Math::Vector2<float> correction = info.normal *
                                          (std::max(info.penetration - slop, 0.0f) /
                                           (bodyA->getInverseMass() + bodyB->getInverseMass())) *
                                          percent;

        if (!bodyA->isStatic())
            bodyA->setPosition(bodyA->getPosition() - correction * bodyA->getInverseMass());

        if (!bodyB->isStatic())
            bodyB->setPosition(bodyB->getPosition() + correction * bodyB->getInverseMass());
    }

    bool PhysicsWorld::checkCollision(Collider *a, Collider *b, CollisionInfo &info)
    {
        info.colliderA = a;
        info.colliderB = b;

        if (a->getType() == ColliderType::Box && b->getType() == ColliderType::Box)
        {
            return checkBoxBox(static_cast<BoxCollider *>(a), static_cast<BoxCollider *>(b), info);
        }
        else if (a->getType() == ColliderType::Circle && b->getType() == ColliderType::Circle)
        {
            return checkCircleCircle(static_cast<CircleCollider *>(a), static_cast<CircleCollider *>(b), info);
        }
        else if (a->getType() == ColliderType::Box && b->getType() == ColliderType::Circle)
        {
            return checkBoxCircle(static_cast<BoxCollider *>(a), static_cast<CircleCollider *>(b), info);
        }
        else if (a->getType() == ColliderType::Circle && b->getType() == ColliderType::Box)
        {
            bool result = checkBoxCircle(static_cast<BoxCollider *>(b), static_cast<CircleCollider *>(a), info);
            if (result)
            {
                info.normal = info.normal * -1.0f;
                std::swap(info.colliderA, info.colliderB);
            }
            return result;
        }

        return false;
    }

    bool PhysicsWorld::checkBoxBox(BoxCollider *a, BoxCollider *b, CollisionInfo &info)
    {
        Math::Vector2<float> posA = a->getWorldPosition();
        Math::Vector2<float> posB = b->getWorldPosition();

        Math::Vector2<float> halfSizeA = a->getHalfSize();
        Math::Vector2<float> halfSizeB = b->getHalfSize();

        Math::Vector2<float> delta = posB - posA;

        float overlapX = halfSizeA.x + halfSizeB.x - std::abs(delta.x);
        float overlapY = halfSizeA.y + halfSizeB.y - std::abs(delta.y);

        if (overlapX < 0 || overlapY < 0)
            return false;

        if (overlapX < overlapY)
        {
            info.normal = Math::Vector2<float>(delta.x < 0 ? -1.0f : 1.0f, 0.0f);
            info.penetration = overlapX;
        }
        else
        {
            info.normal = Math::Vector2<float>(0.0f, delta.y < 0 ? -1.0f : 1.0f);
            info.penetration = overlapY;
        }

        return true;
    }

    bool PhysicsWorld::checkCircleCircle(CircleCollider *a, CircleCollider *b, CollisionInfo &info)
    {
        Math::Vector2<float> posA = a->getWorldPosition();
        Math::Vector2<float> posB = b->getWorldPosition();

        float radiusA = a->getRadius();
        float radiusB = b->getRadius();

        Math::Vector2<float> delta = posB - posA;
        float distance = delta.length();

        if (distance > radiusA + radiusB)
            return false;

        info.normal = delta.normalize();
        info.penetration = radiusA + radiusB - distance;

        return true;
    }

    bool PhysicsWorld::checkBoxCircle(BoxCollider *box, CircleCollider *circle, CollisionInfo &info)
    {
        Math::Vector2<float> boxPos = box->getWorldPosition();
        Math::Vector2<float> circlePos = circle->getWorldPosition();

        Math::Vector2<float> halfSize = box->getHalfSize();
        float radius = circle->getRadius();

        Math::Vector2<float> delta = circlePos - boxPos;

        Math::Vector2<float> closest = delta;

        closest.x = std::max(-halfSize.x, std::min(halfSize.x, closest.x));
        closest.y = std::max(-halfSize.y, std::min(halfSize.y, closest.y));

        bool inside = false;

        if (delta == closest)
        {
            inside = true;

            if (std::abs(delta.x) > std::abs(delta.y))
            {
                closest.x = closest.x > 0 ? halfSize.x : -halfSize.x;
            }
            else
            {
                closest.y = closest.y > 0 ? halfSize.y : -halfSize.y;
            }
        }

        Math::Vector2<float> normal = delta - closest;
        float distance = normal.length();

        if (distance > radius && !inside)
            return false;

        if (distance == 0.0f)
        {
            info.normal = Math::Vector2<float>(1.0f, 0.0f);
            info.penetration = radius;
        }
        else
        {
            info.normal = normal / distance * (inside ? -1.0f : 1.0f);
            info.penetration = inside ? (radius + distance) : (radius - distance);
        }

        return true;
    }
}
