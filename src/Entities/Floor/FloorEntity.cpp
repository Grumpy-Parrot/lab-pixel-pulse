#include "FloorEntity.h"
#include "../../Game/SceneNode.h"
#include "../../Game/Events/UpdateEventPayload.h"
#include "../../Game/Events/StartEventPayload.h"
#include "../../Assets/Image.h"
#include "../../Assets/AssetRegistry.h"
#include "../../Game/Sprite.h"
#include "../../Game/EntityLibrary.h"
#include "../../Physics/PhysicsWorld.h"
#include "../../Logger.h"

namespace PixelPulse::Entities
{
    using namespace PixelPulse::Game;
    using namespace PixelPulse::Game::Events;

    PIXELPULSE_REGISTER_ENTITY(FloorEntity);

    FloorEntity::FloorEntity()
        : m_physicsComponent(nullptr), m_collider(nullptr), m_tileCount(10), m_tileWidth(0.0f), m_floorWidth(2000.0f), m_floorHeight(50.0f)
    {
        m_physicsComponent = PP_NEW(PhysicsComponent, this);
    }

    FloorEntity::~FloorEntity()
    {
        if (m_physicsComponent)
        {
            PP_DELETE(m_physicsComponent);
            m_physicsComponent = nullptr;
        }
    }

    void FloorEntity::onAttach(SceneNode *ownerNode, const AttachEventPayload &payload)
    {
        Logger::info("FloorEntity attached to node: %s", ownerNode->getTag());
        Assets::AssetRegistry *assetRegistry = payload.assetRegistry;

        m_floorImage = assetRegistry->make<Assets::Image>(Assets::AssetMakeRequest{"assets/floor_stone.png"});
        m_floorImage->load();

        m_tileWidth = static_cast<float>(m_floorImage->width);

        Sprite *floorSprite = PP_NEW(Sprite, m_floorImage);
        if (!floorSprite->init(payload.renderer))
        {
            Logger::error("Failed to initialize floor sprite");
            return;
        }

        ownerNode->setSprite(floorSprite);
    }

    void FloorEntity::onStart(SceneNode *ownerNode, const StartEventPayload &payload)
    {
        float scaleX = m_floorWidth / m_tileWidth;
        ownerNode->m_scale = Math::Vector2<float>(scaleX, 1.0f);

        Physics::PhysicsWorld *physicsWorld = payload.physicsWorld;

        Math::Vector2<float> colliderPosition = ownerNode->m_position;
        colliderPosition.y += 60.0f; // The sprite have transparency at the top and bottom, so we need to adjust the collider position

        m_physicsComponent->initialize(physicsWorld, colliderPosition);
        m_physicsComponent->setStatic(true);

        m_collider = m_physicsComponent->createBoxCollider(Math::Vector2<float>(m_floorWidth, m_floorHeight));
    }

    void FloorEntity::onDetach(SceneNode *ownerNode)
    {
        Logger::info("FloorEntity detached from node: %s", ownerNode->getTag());

        Sprite *sprite = ownerNode->getSprite();
        if (sprite)
        {
            PP_DELETE(sprite);
            ownerNode->setSprite(nullptr);
        }
    }
}
