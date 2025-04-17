#include "../Logger.h"
#include "Events/AttachEventPayload.h"
#include "../Utilities.h"
#include "Scene.h"
#include "EntityLibrary.h"

using namespace PixelPulse::Game;

Scene::Scene() : m_rootNode(nullptr)
{
    m_rootNode = new SceneNode();
    m_rootNode->setTag(PIXELPULSE_MAKE_ID_DERIVED("root"));

    m_assetRegistry = nullptr;
    m_renderer = nullptr;
}

Scene::~Scene()
{
    if (m_rootNode)
    {
        delete m_rootNode;
        m_rootNode = nullptr;
    }

    for (IEntity *entity : m_entities)
    {
        delete entity;
    }
    m_entities.clear();
}

void Scene::update(const Events::UpdateEventPayload &payload)
{
    m_rootNode->update(payload, Math::Vector2<float>(0.0f, 0.0f), Math::Vector2<float>(1.0f, 1.0f));
}

void Scene::render(const Game::RenderPassDescriptor &renderPassDescriptor)
{
    m_rootNode->render(m_renderer, &renderPassDescriptor);
}

void Scene::attach(SceneNode *node, SceneNode *parent)
{
    if (!node)
    {
        Logger::error("Attempting to attach a null node");
        return;
    }

    Events::AttachEventPayload attachEventPayload;
    attachEventPayload.assetRegistry = m_assetRegistry;
    attachEventPayload.renderer = m_renderer;

    SceneNode *target = (parent) ? parent : m_rootNode;
    if (target)
    {
        target->addChild(node);
        node->getEntity()->onAttach(node, attachEventPayload);
    }
    else
    {
        Logger::error("Parent node is null, cannot attach node");
        return;
    }
}

SceneNode *Scene::spawn(IEntity *entity)
{
    if (!entity)
    {
        Logger::error("Attempting to spawn a null entity");
        return nullptr;
    }

    SceneNode *node = new SceneNode();
    node->setTag(PIXELPULSE_MAKE_ID());
    node->setEntity(entity);

    m_entities.push_back(entity);

    attach(node);
    return node;
}

SceneNode *Scene::spawnByID(const char *entityID)
{
    if (!entityID)
    {
        Logger::error("Attempting to spawn with a null entity ID");
        return nullptr;
    }

    IEntity *entity = EntityLibrary::getInstance().createEntity(entityID);

    if (!entity)
    {
        Logger::error("Failed to create entity with ID: %s", entityID);
        return nullptr;
    }

    Logger::info("Spawning entity with ID: %s", entityID);
    return spawn(entity);
}
