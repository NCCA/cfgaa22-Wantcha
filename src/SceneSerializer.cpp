#include "SceneSerializer.h"
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include "SceneObject.h"
#include <fstream>
#include <iostream>


void SceneSerializer::Serialize(const std::string& file, NGLScene& scene)
{
    namespace rj=rapidjson;

    rj::StringBuffer s;
    rj::PrettyWriter<rj::StringBuffer> writer(s);

    std::vector<std::shared_ptr<SceneObject>>& objects = scene.getSceneObjects();
    writer.StartObject();

    for(int i = 0; i < objects.size(); i++)
    {
        
        writer.Key(objects[i]->GetName().c_str());
        writer.String("Lmao");
        
        std::cout<<"WROTE";
    }
    writer.EndObject();

    std::ofstream of(file);
    of << s.GetString();
}