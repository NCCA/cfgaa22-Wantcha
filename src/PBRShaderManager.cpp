#include "PBRShaderManager.h"
#include <Lights.h>
#include <fstream>
#include <iostream>

uint32_t PBRShaderManager::s_whiteTextureID = 0;
uint32_t PBRShaderManager::s_blackTextureID = 0;
uint32_t PBRShaderManager::s_blueTextureID = 0;
uint32_t PBRShaderManager::s_shadowMapSize = 2048;
uint32_t PBRShaderManager::s_directionalShadowMap = 0;
uint32_t PBRShaderManager::s_maxDirectionalShadows = 4;

/*PBRShaderManager::PBRShaderManager(const std::string& name, const std::string& vert, const std::string& frag)
    : m_vertPath(vert), m_fragPath(frag), m_name(name)
{

}*/

PBRShaderManager::PBRShaderManager()
{
    CreateWhiteTexture();
}

void PBRShaderManager::CreateWhiteTexture()
{
    glCreateTextures(GL_TEXTURE_2D, 1, &s_whiteTextureID);
	glTextureStorage2D(s_whiteTextureID, 1, GL_RGBA8, 1, 1);

	glTextureParameteri(s_whiteTextureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(s_whiteTextureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTextureParameteri(s_whiteTextureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(s_whiteTextureID, GL_TEXTURE_WRAP_T, GL_REPEAT);


    uint32_t whiteTextureData = 0xffffffff;
    glTextureSubImage2D(s_whiteTextureID, 0, 0, 0, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &whiteTextureData);

    glBindTexture(GL_TEXTURE_2D, s_whiteTextureID);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

}
void PBRShaderManager::CreateBlackTexture()
{
    glCreateTextures(GL_TEXTURE_2D, 1, &s_blackTextureID);
	glTextureStorage2D(s_blackTextureID, 1, GL_RGBA8, 1, 1);

	glTextureParameteri(s_blackTextureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(s_blackTextureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTextureParameteri(s_blackTextureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(s_blackTextureID, GL_TEXTURE_WRAP_T, GL_REPEAT);
    uint32_t blackTextureData = 0x00000000;
    glTextureSubImage2D(s_blackTextureID, 0, 0, 0, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &blackTextureData);

    glBindTexture(GL_TEXTURE_2D, s_blackTextureID);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
}
void PBRShaderManager::CreateBlueTexture()
{
    glCreateTextures(GL_TEXTURE_2D, 1, &s_blueTextureID);
	glTextureStorage2D(s_blueTextureID, 1, GL_RGBA8, 1, 1);

	glTextureParameteri(s_blueTextureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(s_blueTextureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTextureParameteri(s_blueTextureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(s_blueTextureID, GL_TEXTURE_WRAP_T, GL_REPEAT);
    uint32_t blueTextureData = 0x00ff8080;
    glTextureSubImage2D(s_blueTextureID, 0, 0, 0, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &blueTextureData);

    glBindTexture(GL_TEXTURE_2D, s_blueTextureID);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
}

std::string PBRShaderManager::m_fragPath = "";
std::string PBRShaderManager::m_vertPath = "";
std::string PBRShaderManager::m_name = "";
std::vector<std::shared_ptr<Light>> PBRShaderManager::m_directionalLights = std::vector<std::shared_ptr<Light>>();
std::vector<std::shared_ptr<Light>> PBRShaderManager::m_pointLights = std::vector<std::shared_ptr<Light>>();

void PBRShaderManager::Init(const std::string& name,
            const std::string& vert, const std::string& frag)
{
    m_name = name;
    m_vertPath = vert;
    m_fragPath = frag;
    CreateWhiteTexture();
    CreateBlackTexture();
    CreateBlueTexture();

    glGenTextures(1, &s_directionalShadowMap);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, s_directionalShadowMap);
    glTexParameteri(s_directionalShadowMap, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(s_directionalShadowMap, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(s_directionalShadowMap, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glTexParameteri(s_directionalShadowMap, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(s_directionalShadowMap, GL_TEXTURE_WRAP_T, GL_REPEAT);
    /*glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32, s_shadowMapSize, s_shadowMapSize,
            s_maxDirectionalShadows, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);*/
    glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_DEPTH_COMPONENT32, s_shadowMapSize, s_shadowMapSize, s_maxDirectionalShadows);
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

void PBRShaderManager::UpdateLightCounts(std::vector<std::shared_ptr<Light>>& dl, std::vector<std::shared_ptr<Light>>& pl)
{
    // Dynamic light count
    std::fstream file;
    file.open(m_fragPath, std::ios::in| std::ios::out | std::ios::binary );
    std::string fragText;

    const std::string dirCountText = "dirLightCount";
    const std::string pointCountText = "pLightCount";

    m_directionalLights = dl;
    m_pointLights = pl;

    if (file)
    {
        file.seekg(0, std::ios::end);
        fragText.resize(file.tellg());
        file.seekg(0, std::ios::beg);
        file.read(&fragText[0], fragText.size());

        //Directional lights
        size_t pos; 
        pos = fragText.find(dirCountText);

        size_t numberPos = pos + dirCountText.length() + 1;
        std::string numberString = std::to_string( static_cast<int>(dl.size()) );
        std::cout<<numberPos<<std::endl;
        for(int i = 0; i < numberString.length(); ++i)
        {
            file.seekp(numberPos + i);
            file.put( numberString[i] );
        }

        // Point lights
        file.seekg(0, std::ios::beg);
        pos = fragText.find(pointCountText);

        numberPos = pos + pointCountText.length() + 1;
        numberString = std::to_string( static_cast<int>(pl.size()) );
        for(int i = 0; i < numberString.length(); ++i)
        {
            file.seekp(numberPos + i);
            file.put( numberString[i] );
        }

        file.close();
    }
    RefreshCurrentLights();
}

void PBRShaderManager::RefreshCurrentLights()
{
    ngl::ShaderLib::useNullProgram();

    ngl::ShaderLib::loadShader(m_name, m_vertPath, m_fragPath);
    ngl::ShaderLib::use(m_name);

    for(int i = 0; i < m_directionalLights.size(); ++i)
    {
        ngl::ShaderLib::setUniform(("dirLightDirs[" + std::to_string(i) + "]").c_str(),
            m_directionalLights[i]->GetForward() );
        ngl::ShaderLib::setUniform(("dirLightColors[" + std::to_string(i) + "]").c_str(),
            m_directionalLights[i]->GetColor());
        ngl::ShaderLib::setUniform(("dirLightIntensities[" + std::to_string(i) + "]").c_str(),
            m_directionalLights[i]->GetIntensity());
    }

    for(int i = 0; i < m_pointLights.size(); ++i)
    {
        /*ngl::ShaderLib::setUniform(("pLightDirs[" + std::to_string(i) + "]").c_str(),
            pl[i].GetForward() );*/
        ngl::ShaderLib::setUniform(("pLightColors[" + std::to_string(i) + "]").c_str(),
            m_pointLights[i]->GetColor());

        ngl::ShaderLib::setUniform(("pLightIntensities[" + std::to_string(i) + "]").c_str(),
            m_pointLights[i]->GetIntensity());
        ngl::ShaderLib::setUniform(("pLightPos[" + std::to_string(i) + "]").c_str(),
            m_pointLights[i]->GetPosition());
    }
}

void PBRShaderManager::UseShader()
{
    ngl::ShaderLib::use(m_name);
}