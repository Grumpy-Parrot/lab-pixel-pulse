#include "PlayerEntity.h"
#include "../../Game/SceneNode.h"
#include "../../Game/Events/UpdateEventPayload.h"
#include "../../Game/Input.h"
#include "../../Assets/Image.h"
#include "../../Game/Sprite.h"
#include "../../Game/EntityLibrary.h"

using namespace PixelPulse::Entities;
using namespace PixelPulse::Game;
using namespace PixelPulse::Game::Events;

PIXELPULSE_REGISTER_ENTITY(PlayerEntity)

PlayerEntity::PlayerEntity()
{
    m_moveSpeed = 400.0f;
}

void PlayerEntity::onUpdate(SceneNode *ownerNode, const UpdateEventPayload &payload)
{
    Input *input = payload.input;
    float deltaTime = payload.deltaTime;

    Math::Vector2<float> movementDelta(0.0f, 0.0f);

    if (input->isKeyPressed(KeyCode::KEY_W))
    {
        movementDelta.y -= m_moveSpeed * deltaTime;
    }
    if (input->isKeyPressed(KeyCode::KEY_S))
    {
        movementDelta.y += m_moveSpeed * deltaTime;
    }
    if (input->isKeyPressed(KeyCode::KEY_A))
    {
        movementDelta.x -= m_moveSpeed * deltaTime;
    }
    if (input->isKeyPressed(KeyCode::KEY_D))
    {
        movementDelta.x += m_moveSpeed * deltaTime;
    }

    ownerNode->m_position += movementDelta;
}

void PlayerEntity::onAttach(SceneNode *ownerNode, const AttachEventPayload &payload)
{
    Logger::info("PlayerEntity attached to node: %s", ownerNode->getTag());
    AssetRegistry *assetRegistry = payload.assetRegistry;

    Image *vampireImage = assetRegistry->make<Image>(AssetMakeRequest{"assets/vampire.png"});
    vampireImage->load();

    Sprite *vampireSprite = PP_NEW(Sprite, vampireImage);
    if (!vampireSprite->init(payload.renderer))
    {
        Logger::error("Failed to initialize sprite");
        return;
    }

    ownerNode->setSprite(vampireSprite);
    ownerNode->m_position = Math::Vector2<float>(100, 100);
    ownerNode->m_scale = Math::Vector2<float>(0.2f, 0.2f);
}

void PlayerEntity::onDetach(SceneNode *ownerNode)
{
    Logger::info("PlayerEntity detached from node: %s", ownerNode->getTag());

    Sprite *sprite = ownerNode->getSprite();
    if (sprite)
    {
        PP_DELETE(sprite);
        ownerNode->setSprite(nullptr);
    }
}
