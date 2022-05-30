#ifndef SCENE_SERIALIZER_H
#define SCENE_SERIALIZER_H

#include "NGLScene.h"

class SceneSerializer
{
public:
    static void Serialize(const std::string& file, NGLScene& scene);
    static void Deserialize(const std::string& file, NGLScene& scene);
};

#endif