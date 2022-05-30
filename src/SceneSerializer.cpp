#include "SceneSerializer.h"
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/istreamwrapper.h>
#include "SceneObject.h"
#include "Lights.h"
#include "MeshObject.h"
#include <fstream>
#include <iostream>


void SceneSerializer::Serialize(const std::string& file, NGLScene& scene)
{
    namespace rj=rapidjson;

    rj::StringBuffer s;
    rj::PrettyWriter<rj::StringBuffer> writer(s);

    std::vector<std::shared_ptr<SceneObject>>& objects = scene.getSceneObjects();

    writer.StartObject();
    writer.Key("Lightweight OpenGL Loader Version");
    writer.Double(1.0f);

    writer.Key("EnvironmentTexture");
    writer.String(scene.getEnvironmentMap()->GetFilepath().c_str());
    writer.Key("RenderEnvironment");
    writer.Bool(scene.getRenderEnvironment());
    writer.Key("AmbientIntensity");
    writer.Double(scene.getAmbientIntensity());
    writer.Key("BackgroundColor");
    writer.StartArray();           
    writer.Double(scene.GetBackgroundColor().m_x);
    writer.Double(scene.GetBackgroundColor().m_y);
    writer.Double(scene.GetBackgroundColor().m_z);
    writer.EndArray();

    writer.Key("SceneObjects");
    writer.StartArray();

    for(int i = 0; i < objects.size(); i++)
    {
        writer.StartObject();
        writer.Key("Name"); writer.Key(objects[i]->GetName().c_str());
        writer.Key("Position");
        writer.StartArray();
        writer.Double(objects[i]->GetPosition().m_x);
        writer.Double(objects[i]->GetPosition().m_y);
        writer.Double(objects[i]->GetPosition().m_z);
        writer.EndArray();

        writer.Key("Rotation");
        writer.StartArray();
        writer.Double(objects[i]->GetRotation().m_x);
        writer.Double(objects[i]->GetRotation().m_y);
        writer.Double(objects[i]->GetRotation().m_z);
        writer.EndArray();

        writer.Key("Scale");
        writer.StartArray();
        writer.Double(objects[i]->GetScale().m_x);
        writer.Double(objects[i]->GetScale().m_y);
        writer.Double(objects[i]->GetScale().m_z);
        writer.EndArray();

        writer.Key("ObjectType"); 
        if(!objects[i]->IsLight())
        {
            std::shared_ptr<Mesh> mesh = objects[i]->GetMesh();
            writer.String("Mesh");

            writer.Key("MeshFilepath");
            writer.Key(AssetManager::GetPath<std::shared_ptr<ObjMesh>>(std::dynamic_pointer_cast<ObjMesh>(mesh)).c_str());

            writer.Key("AlbedoTexture");
            writer.String(AssetManager::GetPath<std::shared_ptr<ngl::Texture>>(objects[i]->GetMaterial().m_albedoTexture).c_str());

            writer.Key("RoughnessTexture");
            writer.String(AssetManager::GetPath<std::shared_ptr<ngl::Texture>>(objects[i]->GetMaterial().m_roughnessTexture).c_str());

            writer.Key("NormalTexture");
            writer.String(AssetManager::GetPath<std::shared_ptr<ngl::Texture>>(objects[i]->GetMaterial().m_normalTexture).c_str());

            writer.Key("AOTexture");
            writer.String(AssetManager::GetPath<std::shared_ptr<ngl::Texture>>(objects[i]->GetMaterial().m_aoTexture).c_str());

            writer.Key("MetallicTexture");
            writer.String(AssetManager::GetPath<std::shared_ptr<ngl::Texture>>(objects[i]->GetMaterial().m_metallicTexture).c_str());

            writer.Key("RoughnessValue");
            writer.Double(objects[i]->GetMaterial().m_roughness);
            
            writer.Key("MetallicValue");
            writer.Double(objects[i]->GetMaterial().m_metallic);
        }
        else
        {
            std::shared_ptr<Light> light = std::dynamic_pointer_cast<Light>(objects[i]);
            if(light->GetType() == LightType::Directional)
            {
                writer.String("DirectionalLight");
            }
            else
            {
                writer.String("PointLight");
            }
            writer.Key("Intensity"); writer.Double(light->GetIntensity());
            writer.Key("Color");
            writer.StartArray();           
            writer.Double(light->GetColor().m_x);
            writer.Double(light->GetColor().m_y);
            writer.Double(light->GetColor().m_z);
            writer.EndArray();
        }
        writer.EndObject();
    }
    writer.EndArray();

    writer.EndObject();

    std::ofstream of(file);
    of << s.GetString();
}

void SceneSerializer::Deserialize(const std::string& file, NGLScene& scene)
{
    namespace rj=rapidjson;
    rj::Document d;

    std::ifstream ifs(file);
    rj::IStreamWrapper isw(ifs);

    d.ParseStream(isw);
    d.IsObject();
    if(d.HasMember("Lightweight OpenGL Loader Version"))
    {
        scene.DeselectAll();
        scene.makeCurrent();
        scene.resetScene();

        if( d["EnvironmentTexture"].GetStringLength() != 0 )
        {
            scene.getEnvironmentMap()->SetTexture(d["EnvironmentTexture"].GetString());
        }
        else
        {
            scene.getEnvironmentMap()->SetTexture(PBRShaderManager::s_whiteTextureID);
        }
        scene.setRenderEnvironment(d["RenderEnvironment"].GetBool());
        scene.setAmbientIntensity(d["AmbientIntensity"].GetDouble());
        const rj::Value& bg = d["BackgroundColor"];
        scene.SetBackgroundColor( ngl::Vec3(bg[0].GetDouble(), bg[1].GetDouble(), bg[2].GetDouble() ));

        const rj::Value& objects = d["SceneObjects"];
        for (rj::SizeType i = 0; i < objects.Size(); i++)
        {
            std::string objectType = objects[i]["ObjectType"].GetString();
            std::vector<std::shared_ptr<SceneObject>>& sceneObjects = scene.getSceneObjects();

            switch (objectType[0])
            {
                case 'M':
                    scene.OnAddMesh(objects[i]["MeshFilepath"].GetString());
                    //scene.makeCurrent();
                    if(objects[i]["AlbedoTexture"].GetStringLength() != 0)
                    {
                        sceneObjects[sceneObjects.size() - 1]->GetMaterial().SetTexture(TextureType::ALBEDO,
                                        AssetManager::GetAsset<ngl::Texture>(objects[i]["AlbedoTexture"].GetString()));
                    }

                    if(objects[i]["RoughnessTexture"].GetStringLength() != 0)
                        sceneObjects[sceneObjects.size() - 1]->GetMaterial().SetTexture(TextureType::ROUGHNESS,
                                        AssetManager::GetAsset<ngl::Texture>(objects[i]["RoughnessTexture"].GetString()));

                    if(objects[i]["NormalTexture"].GetStringLength() != 0)
                        sceneObjects[sceneObjects.size() - 1]->GetMaterial().SetTexture(TextureType::NORMAL,
                                        AssetManager::GetAsset<ngl::Texture>(objects[i]["NormalTexture"].GetString()));

                    if(objects[i]["AOTexture"].GetStringLength() != 0)
                        sceneObjects[sceneObjects.size() - 1]->GetMaterial().SetTexture(TextureType::AO,
                                        AssetManager::GetAsset<ngl::Texture>(objects[i]["AOTexture"].GetString()));
                    if(objects[i]["MetallicTexture"].GetStringLength() != 0)
                    {
                        sceneObjects[sceneObjects.size() - 1]->GetMaterial().SetTexture(TextureType::METALLIC,
                                        AssetManager::GetAsset<ngl::Texture>(objects[i]["MetallicTexture"].GetString()));
                    }

                    sceneObjects[sceneObjects.size() - 1]->GetMaterial().m_roughness = objects[i]["RoughnessValue"].GetDouble();
                    sceneObjects[sceneObjects.size() - 1]->GetMaterial().m_metallic = objects[i]["MetallicValue"].GetDouble();

                    break;
                case 'D':
                {
                    scene.OnAddDirectionalLight();
                    std::shared_ptr<Light> light1 = std::dynamic_pointer_cast<Light>(sceneObjects[sceneObjects.size() - 1]);

                    const rj::Value& color1 = objects[i]["Color"];
                    light1->SetColor( ngl::Vec3( color1[0].GetDouble(), color1[1].GetDouble(), color1[2].GetDouble() ) );
                    light1->SetIntensity( objects[i]["Intensity"].GetDouble() );
                    break;
                }
                case 'P':
                {
                    scene.OnAddPointLight();

                    std::shared_ptr<Light> light2 = std::dynamic_pointer_cast<Light>(sceneObjects[sceneObjects.size() - 1]);

                    const rj::Value& color2 = objects[i]["Color"];
                    light2->SetColor( ngl::Vec3( color2[0].GetDouble(), color2[1].GetDouble(), color2[2].GetDouble() ) );
                    light2->SetIntensity( objects[i]["Intensity"].GetDouble() );
                    break;
                }
            }
            sceneObjects[sceneObjects.size() - 1]->SetName(objects[i]["Name"].GetString());
            const rj::Value& pos = objects[i]["Position"];
            const rj::Value& rot = objects[i]["Rotation"];
            const rj::Value& scale = objects[i]["Scale"];
            sceneObjects[sceneObjects.size() - 1]->SetPosition(ngl::Vec3( pos[0].GetDouble(), pos[1].GetDouble(), pos[2].GetDouble() ));
            sceneObjects[sceneObjects.size() - 1]->SetRotation(ngl::Vec3( rot[0].GetDouble(), rot[1].GetDouble(), rot[2].GetDouble() ));
            sceneObjects[sceneObjects.size() - 1]->SetScale(ngl::Vec3( scale[0].GetDouble(), scale[1].GetDouble(), scale[2].GetDouble() ));
        }

        scene.update();
        scene.doneCurrent();
    }
    else
    {
        std::cout<<"ERROR! INVALID FILE!\n";
    }
}
