#include "SceneLoader.h"
#include "../Logger.h"
#include "../Libraries/Libraries.h"
#include "../Game/SceneNode.h"
#include "../Libraries/JSON.h"

using json = nlohmann::json;

namespace PixelPulse::Game
{
    SceneLoader::SceneLoader()
    {
    }

    SceneLoader::~SceneLoader()
    {
    }

    bool SceneLoader::loadScene(Scene *scene, const char *jsonFilePath)
    {
        if (!scene)
        {
            Logger::error("SceneLoader: Cannot load into null scene");
            return false;
        }

        if (!jsonFilePath)
        {
            Logger::error("SceneLoader: JSON file path is null");
            return false;
        }

        std::ifstream file(jsonFilePath);
        if (!file.is_open())
        {
            Logger::error("SceneLoader: Failed to open file: %s", jsonFilePath);
            return false;
        }

        try
        {
            std::string jsonStr((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            file.close();

            json sceneJson = json::parse(jsonStr);

            if (sceneJson.contains("entities") && sceneJson["entities"].is_array())
            {
                for (const auto &entityJson : sceneJson["entities"])
                {
                    if (!parseEntity(scene, entityJson.dump()))
                    {
                        Logger::warning("SceneLoader: Failed to parse an entity");
                    }
                }
            }
            else
            {
                Logger::warning("SceneLoader: JSON missing 'entities' array");
            }

            Logger::info("SceneLoader: Successfully loaded scene from: %s", jsonFilePath);
            return true;
        }
        catch (const json::parse_error &e)
        {
            Logger::error("SceneLoader: JSON parse error: %s", e.what());
            return false;
        }
        catch (const std::exception &e)
        {
            Logger::error("SceneLoader: Error loading scene: %s", e.what());
            return false;
        }
    }

    bool SceneLoader::parseEntities(Scene *scene, const std::string &entitiesJson)
    {
        try
        {
            json entities = json::parse(entitiesJson);

            if (!entities.is_array())
            {
                Logger::error("SceneLoader: Entities JSON is not an array");
                return false;
            }

            for (const auto &entityJson : entities)
            {
                if (!parseEntity(scene, entityJson.dump()))
                {
                    Logger::warning("SceneLoader: Failed to parse an entity");
                }
            }

            return true;
        }
        catch (const json::parse_error &e)
        {
            Logger::error("SceneLoader: JSON parse error in entities: %s", e.what());
            return false;
        }
    }

    SceneNode *SceneLoader::parseEntity(Scene *scene, const std::string &entityJson)
    {
        try
        {
            json entity = json::parse(entityJson);

            if (!entity.contains("type") || !entity["type"].is_string())
            {
                Logger::error("SceneLoader: Entity JSON missing required 'type' field");
                return nullptr;
            }

            std::string entityType = entity["type"];

            SceneNode *node = scene->spawnByID(entityType.c_str());
            if (!node)
            {
                Logger::error("SceneLoader: Failed to spawn entity of type: %s", entityType.c_str());
                return nullptr;
            }

            if (entity.contains("position") && entity["position"].is_object() &&
                entity["position"].contains("x") && entity["position"].contains("y"))
            {
                node->m_position.x = entity["position"]["x"].get<float>();
                node->m_position.y = entity["position"]["y"].get<float>();
            }

            if (entity.contains("scale") && entity["scale"].is_object() &&
                entity["scale"].contains("x") && entity["scale"].contains("y"))
            {
                node->m_scale.x = entity["scale"]["x"].get<float>();
                node->m_scale.y = entity["scale"]["y"].get<float>();
            }

            if (entity.contains("rotation") && entity["rotation"].is_number())
            {
                node->m_rotation = entity["rotation"].get<float>();
            }

            if (entity.contains("tag") && entity["tag"].is_string())
            {
                std::string tagStr = entity["tag"].get<std::string>();
                char *tag = PP_NEW_ARRAY(char, tagStr.length() + 1);
#ifdef PLATFORM_WINDOWS
                strcpy_s(tag, tagStr.length() + 1, tagStr.c_str());
#else
                std::strcpy(tag, tagStr.c_str());
#endif
                node->setTag(tag);
            }

            return node;
        }
        catch (const json::parse_error &e)
        {
            Logger::error("SceneLoader: JSON parse error in entity: %s", e.what());
            return nullptr;
        }
        catch (const std::exception &e)
        {
            Logger::error("SceneLoader: Error parsing entity: %s", e.what());
            return nullptr;
        }
    }
}
