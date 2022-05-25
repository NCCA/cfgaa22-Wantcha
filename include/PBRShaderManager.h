#ifndef PBR_SHADER_MANAGER_H
#define PBR_SHADER_MANAGER_H

#include <ngl/ShaderLib.h>
#include <string>
#include <vector>
#include <ngl/Texture.h>
#include <memory>

class Light;

enum class TextureType { ALBEDO, NORMAL, ROUGHNESS, AO, METALLIC };

class PBRShaderManager
{

public:

    PBRShaderManager();
    ~PBRShaderManager() = default;

    //PBRShaderManager (const std::string& name, const std::string& vert, const std::string& frag);

    static void Init(const std::string& name,
            const std::string& vert, const std::string& frag);

    static void UpdateLightCounts(std::vector<Light>& dl,
                                    std::vector<Light>& pl);
    
    static void UseShader();

    static uint32_t s_whiteTextureID;
    static uint32_t s_blackTextureID;
    static uint32_t s_blueTextureID;
private:
    static std::string m_fragPath;
    static std::string m_vertPath;
    static std::string m_name;
    static void CreateWhiteTexture();
    static void CreateBlackTexture();
    static void CreateBlueTexture();
};

struct Material
{
    //Material() = default;
    //Material(const Material&);

    void SetTexture(TextureType type, const std::string& path)
    {
        switch(type)
        {
            case TextureType::ALBEDO:
                m_albedoTexture = std::make_shared<ngl::Texture>(path);
                m_albedoTexture->setMultiTexture(0);
                m_albedoID = m_albedoTexture->setTextureGL();
                break;

            case TextureType::ROUGHNESS:
                m_roughnessTexture = std::make_shared<ngl::Texture>(path);
                m_roughnessTexture->setMultiTexture(0);
                m_roughnessID = m_roughnessTexture->setTextureGL();
                break;

            case TextureType::NORMAL:
                m_normalTexture = std::make_shared<ngl::Texture>(path);
                m_normalTexture->setMultiTexture(0);
                m_normalID = m_normalTexture->setTextureGL();
                    break;

            case TextureType::AO:
                m_aoTexture = std::make_shared<ngl::Texture>(path);
                m_aoTexture->setMultiTexture(0);
                m_aoID = m_aoTexture->setTextureGL();
                    break;

            case TextureType::METALLIC:
                m_metallicTexture = std::make_shared<ngl::Texture>(path);
                m_metallicTexture->setMultiTexture(0);
                m_metallicID = m_metallicTexture->setTextureGL();
                    break;
        }
        // mip map the textures
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    void BindTextures() const
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, PBRShaderManager::s_whiteTextureID);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_albedoID);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, m_roughnessID);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, m_normalID);

        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, m_aoID);

        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, m_metallicID);
    }

    ngl::Vec4 m_baseColor = {1.0f, 1.0f, 1.0f, 1.0f};

    std::shared_ptr<ngl::Texture> m_albedoTexture;
    std::shared_ptr<ngl::Texture> m_roughnessTexture;
    std::shared_ptr<ngl::Texture> m_normalTexture;
    std::shared_ptr<ngl::Texture> m_aoTexture;
    std::shared_ptr<ngl::Texture> m_metallicTexture;

    GLuint m_albedoID = PBRShaderManager::s_whiteTextureID;
    GLuint m_roughnessID = PBRShaderManager::s_whiteTextureID;
    GLuint m_normalID = PBRShaderManager::s_blueTextureID;
    GLuint m_aoID = PBRShaderManager::s_whiteTextureID;
    GLuint m_metallicID = PBRShaderManager::s_blackTextureID;

    float m_roughness = 0.5f;
    float m_specular = 0.5f;
    float m_metallic = 1.0f;
};

#endif