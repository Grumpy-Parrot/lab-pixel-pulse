#include "EnemyEntity.h"
#include "../../Game/SceneNode.h"
#include "../../Game/Events/UpdateEventPayload.h"
#include "../../Game/Input.h"
#include "../../Assets/Image.h"
#include "../../Game/Sprite.h"
#include "../../Game/EntityLibrary.h"

using namespace PixelPulse::Entities;
using namespace PixelPulse::Game;
using namespace PixelPulse::Game::Events;

PIXELPULSE_REGISTER_ENTITY(EnemyEntity)

EnemyEntity::EnemyEntity()
{
    m_moveSpeed = 100.0f;
}

void EnemyEntity::onUpdate(SceneNode *ownerNode, const UpdateEventPayload &payload)
{
    PIXELPULSE_ARG_UNUSED(ownerNode);
    PIXELPULSE_ARG_UNUSED(payload);
}

void EnemyEntity::onAttach(SceneNode *ownerNode, const AttachEventPayload &payload)
{
    Logger::info("EnemyEntity attached to node: %s", ownerNode->getTag());
    AssetRegistry *assetRegistry = payload.assetRegistry;

    Image *skeletonImage = assetRegistry->make<Image>(AssetMakeRequest{"assets/skeleton.png"});
    skeletonImage->load();

    Sprite *skeletonSprite = PP_NEW(Sprite, skeletonImage);
    if (!skeletonSprite->init(payload.renderer))
    {
        Logger::error("Failed to initialize sprite");
        return;
    }

    ownerNode->setSprite(skeletonSprite);
    ownerNode->m_position = Math::Vector2<float>(0, 0);
    ownerNode->m_scale = Math::Vector2<float>(0.2f, 0.2f);
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
