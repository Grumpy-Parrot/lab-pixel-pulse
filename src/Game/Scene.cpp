#include "Scene.h"
#include "../Logger.h"
#include "Events/AttachEventPayload.h"
#include "../Utilities.h"
#include "EntityLibrary.h"
#include "SceneLoader.h"

namespace PixelPulse::Game
{
    Scene::Scene() : m_rootNode(nullptr)
    {
        m_rootNode = PP_NEW(SceneNode);
        m_rootNode->setTag(PIXELPULSE_MAKE_ID_DERIVED("root"));

        m_assetRegistry = nullptr;
        m_renderer = nullptr;
        m_physicsWorld = nullptr;
    }

    Scene::~Scene()
    {
        if (m_rootNode)
        {
            PP_DELETE(m_rootNode);
            m_rootNode = nullptr;
        }

        for (IEntity *entity : m_entities)
        {
            PP_DELETE(entity);
        }
        m_entities.clear();
    }

    void Scene::start()
    {
        if (m_rootNode)
        {
            Events::StartEventPayload startEventPayload;
            startEventPayload.assetRegistry = m_assetRegistry;
            startEventPayload.renderer = m_renderer;
            startEventPayload.physicsWorld = m_physicsWorld;

            m_rootNode->start(startEventPayload);
        }
        else
        {
            Logger::error("Scene::start: Root node is null");
        }
    }

    void Scene::update(const Events::UpdateEventPayload &payload)
    {
        Events::UpdateEventPayload updatedPayload = payload;
        updatedPayload.physicsWorld = m_physicsWorld;

        m_rootNode->update(updatedPayload, Math::Vector2<float>(0.0f, 0.0f), Math::Vector2<float>(1.0f, 1.0f));
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
        attachEventPayload.physicsWorld = m_physicsWorld;

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

        SceneNode *node = PP_NEW(SceneNode);
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

    bool Scene::loadFromJSON(const char *jsonFilePath)
    {
        if (!jsonFilePath)
        {
            Logger::error("Scene::loadFromJSON: JSON file path is null");
            return false;
        }

        if (!m_renderer)
        {
            Logger::error("Scene::loadFromJSON: Renderer is not set");
            return false;
        }

        if (!m_assetRegistry)
        {
            Logger::error("Scene::loadFromJSON: AssetRegistry is not set");
            return false;
        }

        const char *appPath = SDL_GetBasePath();
        char *jsonFilePathAbsolute = Platform::String::join(appPath, jsonFilePath);

        SceneLoader sceneLoader;
        bool result = sceneLoader.loadScene(this, jsonFilePathAbsolute);

        if (!result)
        {
            Logger::error("Scene::loadFromJSON: Failed to load scene from: %s", jsonFilePathAbsolute);
        }
        else
        {
            Logger::info("Scene::loadFromJSON: Successfully loaded scene from: %s", jsonFilePathAbsolute);
        }

        Platform::String::free(jsonFilePathAbsolute);

        return result;
    }
}
