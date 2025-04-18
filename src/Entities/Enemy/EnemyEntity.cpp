#include "EnemyEntity.h"
#include "../../Game/SceneNode.h"
#include "../../Game/Events/UpdateEventPayload.h"
#include "../../Game/Input.h"
#include "../../Assets/Image.h"
#include "../../Game/Sprite.h"
#include "../../Game/EntityLibrary.h"

namespace PixelPulse::Entities
{
    using namespace PixelPulse::Game;
    using namespace PixelPulse::Game::Events;

    PIXELPULSE_REGISTER_ENTITY(EnemyEntity);

    EnemyEntity::EnemyEntity()
    {
        m_moveSpeed = 100.0f;
        m_physicsComponent = PP_NEW(PhysicsComponent, this);
    }

    EnemyEntity::~EnemyEntity()
    {
        if (m_physicsComponent)
        {
            PP_DELETE(m_physicsComponent);
            m_physicsComponent = nullptr;
        }
    }

    void EnemyEntity::onUpdate(SceneNode *ownerNode, const UpdateEventPayload &payload)
    {
        PIXELPULSE_ARG_UNUSED(payload);

        Math::Vector2<float> position = m_physicsComponent->getPosition();
        ownerNode->m_position = position;
    }

    void EnemyEntity::onAttach(SceneNode *ownerNode, const AttachEventPayload &payload)
    {
        Logger::info("EnemyEntity attached to node: %s", ownerNode->getTag());
        Assets::AssetRegistry *assetRegistry = payload.assetRegistry;

        m_enemyImage = assetRegistry->make<Assets::Image>(Assets::AssetMakeRequest{"assets/skeleton.png"});
        m_enemyImage->load();

        Sprite *skeletonSprite = PP_NEW(Sprite, m_enemyImage);
        if (!skeletonSprite->init(payload.renderer))
        {
            Logger::error("Failed to initialize sprite");
            return;
        }

        ownerNode->setSprite(skeletonSprite);
    }

    void EnemyEntity::onStart(SceneNode *ownerNode, const StartEventPayload &payload)
    {
        if (!m_physicsComponent)
        {
            Logger::error("EnemyEntity::onStart - Physics component is null");
            return;
        }

        m_physicsComponent->initialize(payload.physicsWorld, ownerNode->m_position);
        m_collider = m_physicsComponent->createBoxCollider(Math::Vector2<float>(50.0f, 50.0f));
    }

    void EnemyEntity::onDetach(SceneNode *ownerNode)
    {
        Logger::info("EnemyEntity detached from node: %s", ownerNode->getTag());

        Sprite *sprite = ownerNode->getSprite();
        if (sprite)
        {
            PP_DELETE(sprite);
            ownerNode->setSprite(nullptr);
        }
    }

    void EnemyEntity::onCollision(PixelPulse::Physics::Collider *collider, const PixelPulse::Physics::CollisionInfo &info)
    {
        PIXELPULSE_ARG_UNUSED(collider);
        PIXELPULSE_ARG_UNUSED(info);

        Logger::info("EnemyEntity collided with another collider");
    }
}
