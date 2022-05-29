#ifndef PBR_SHADER_MANAGER_H
#define PBR_SHADER_MANAGER_H

#include <ngl/ShaderLib.h>
#include <string>
#include <vector>
#include <ngl/Texture.h>
#include <EnvironmentTexture.h>
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

    static void UpdateLightCounts();

    static void RefreshCurrentLights();
    static std::shared_ptr<Light> AddDirectionalLight(const ngl::Vec3& position = { 0, 1, 0 }, const ngl::Vec3& rotation = { 90, 0, 0 },
                const ngl::Vec3& color = {1,1,1}, float intensity = 1.0f);
    static std::shared_ptr<Light> AddPointLight(const ngl::Vec3& position = { 0, 1, 0 },
                const ngl::Vec3& color = {1,1,1}, float intensity = 1.0f);

    static void RemoveDirectionalLight( std::shared_ptr<Light> light );
    static void RemovePointLight( std::shared_ptr<Light> light );
    
    static void UseShader();
    //static void DrawQuad() { s_quadMesh->bind(); s_quadMesh->draw(); s_quadMesh->unbind(); }

    static uint32_t s_whiteTextureID;
    static uint32_t s_blackTextureID;
    static uint32_t s_blueTextureID;
    static uint32_t s_shadowMapSize;
    static uint32_t s_maxDirectionalShadows;
    static uint32_t s_curDirShadowIndex;

    static uint32_t s_maxPointShadows;
    static uint32_t s_curPointShadowIndex;

    static uint32_t s_directionalShadowMap;
    static uint32_t s_pointShadowMap;
    static std::unique_ptr<FrameBuffer> s_pointShadowBuffer;
    //static EnvironmentTexture s_envMap;
    static std::vector<std::shared_ptr<Light>> s_directionalLights;
    static std::vector<std::shared_ptr<Light>> s_pointLights;

    //static std::unique_ptr<ngl::AbstractVAO> s_quadMesh;

private:
    //static SimpleVertData m_vbo[6];
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