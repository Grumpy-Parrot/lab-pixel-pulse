#pragma once

#ifndef PIXELPULSE_SCENELOADER_H
#define PIXELPULSE_SCENELOADER_H

#include "Scene.h"
#include "../Platform/Std.h"

namespace PixelPulse::Game
{
    class SceneLoader
    {
    public:
        SceneLoader();
        ~SceneLoader();

        bool loadScene(Scene *scene, const char *jsonFilePath);

    private:
        bool parseEntities(Scene *scene, const std::string &entitiesJson);
        SceneNode *parseEntity(Scene *scene, const std::string &entityJson);
    };
}

#endif
