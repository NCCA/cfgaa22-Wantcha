#ifndef PBR_SHADER_MANAGER_H
#define PBR_SHADER_MANAGER_H

#include <ngl/ShaderLib.h>
#include <string>
#include <vector>
#include <Lights.h>
#include <ngl/Texture.h>
#include <memory>

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
private:
    static std::string m_fragPath;
    static std::string m_vertPath;
    static std::string m_name;
    static void CreateWhiteTexture();
};

struct Material
{
    //Material() = default;
    //Material(const Material&);

    void SetTexture(const std::string& path)
    {
        m_albedoTexture = std::make_unique<ngl::Texture>(path);
        m_albedoTexture->setMultiTexture(0);
        m_albedoID = m_albedoTexture->setTextureGL();
          // mip map the textures
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    void BindTextures() const
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, PBRShaderManager::s_whiteTextureID);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    ngl::Vec4 m_baseColor = {1.0f, 1.0f, 1.0f, 1.0f};

    std::unique_ptr<ngl::Texture> m_albedoTexture;
    GLuint m_albedoID = PBRShaderManager::s_whiteTextureID;

    float m_roughness = 0.5f;
    float m_specular = 0.5f;
    float m_metallic = 1.0f;
};

#endif