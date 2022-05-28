#include "PBRShaderManager.h"
#include <Lights.h>
#include <fstream>
#include <iostream>
#include <../3rdparty/stb_image.h>

uint32_t PBRShaderManager::s_whiteTextureID = 0;
uint32_t PBRShaderManager::s_blackTextureID = 0;
uint32_t PBRShaderManager::s_blueTextureID = 0;
uint32_t PBRShaderManager::s_shadowMapSize = 2048;
uint32_t PBRShaderManager::s_directionalShadowMap = 0;
uint32_t PBRShaderManager::s_pointShadowMap = 0;
uint32_t PBRShaderManager::s_maxDirectionalShadows = 4;
uint32_t PBRShaderManager::s_curDirShadowIndex = 0;

uint32_t PBRShaderManager::s_maxPointShadows = 4;
uint32_t PBRShaderManager::s_curPointShadowIndex = 0;
std::unique_ptr<FrameBuffer> PBRShaderManager::s_pointShadowBuffer;

EnvironmentTexture PBRShaderManager::s_envMap;

/*PBRShaderManager::PBRShaderManager(const std::string& name, const std::string& vert, const std::string& frag)
    : m_vertPath(vert), m_fragPath(frag), m_name(name)
{

}*/

PBRShaderManager::PBRShaderManager()
{
    //CreateWhiteTexture();
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
std::vector<std::shared_ptr<Light>> PBRShaderManager::s_directionalLights = std::vector<std::shared_ptr<Light>>();
std::vector<std::shared_ptr<Light>> PBRShaderManager::s_pointLights = std::vector<std::shared_ptr<Light>>();

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
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);
    /*glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32, s_shadowMapSize, s_shadowMapSize,
            s_maxDirectionalShadows, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);*/
    glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_DEPTH_COMPONENT32, s_shadowMapSize, s_shadowMapSize, s_maxDirectionalShadows);
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);


    glGenTextures(1, &s_pointShadowMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, s_pointShadowMap);
    /*for(int i = 0; i < 6; i++)
    {
        glTexImage3D(GL_TEXTURE_CUBE_MAP)
    }*/
    glTexStorage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 1, GL_DEPTH_COMPONENT32, s_shadowMapSize, s_shadowMapSize, s_maxPointShadows * 6);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);

    s_pointShadowBuffer.reset( new FrameBuffer(s_pointShadowMap) );
    std::cout<<"Loading HDRI\n";
    //s_envMap->loadImage("textures/HDRI/Factory_Catwalk_Env.hdr");
    

    ngl::ShaderLib::useNullProgram();

    ngl::ShaderLib::loadShader(m_name, m_vertPath, m_fragPath);
}

void PBRShaderManager::UpdateLightCounts()
{
    // Dynamic light count
    std::fstream fragFile, vertFile;
    fragFile.open(m_fragPath, std::ios::in| std::ios::out | std::ios::binary );
    vertFile.open(m_vertPath, std::ios::in| std::ios::out | std::ios::binary );
    std::string fragText, vertText;

    const std::string dirCountText = "dirLightCount";
    const std::string pointCountText = "pLightCount";

    if (fragFile)
    {
        fragFile.seekg(0, std::ios::end);
        fragText.resize(fragFile.tellg());
        fragFile.seekg(0, std::ios::beg);
        fragFile.read(&fragText[0], fragText.size());

        vertFile.seekg(0, std::ios::end);
        vertText.resize(vertFile.tellg());
        vertFile.seekg(0, std::ios::beg);
        vertFile.read(&vertText[0], vertText.size());

        //Directional lights
        size_t posFrag, posVert; 
        posFrag = fragText.find(dirCountText);
        posVert = vertText.find(dirCountText);

        size_t numberPosFrag = posFrag + dirCountText.length() + 1;
        size_t numberPosVert = posVert + dirCountText.length() + 1;
        std::string numberString = std::to_string( static_cast<int>(s_directionalLights.size()) );

        for(int i = 0; i < numberString.length(); ++i)
        {
            fragFile.seekp(numberPosFrag + i);
            fragFile.put( numberString[i] );

            vertFile.seekp(numberPosVert + i);
            vertFile.put( numberString[i] );
        }


        // Point lights
        fragFile.seekg(0, std::ios::beg);
        posFrag = fragText.find(pointCountText);

        numberPosFrag = posFrag + pointCountText.length() + 1;
        numberString = std::to_string( static_cast<int>(s_pointLights.size()) );
        for(int i = 0; i < numberString.length(); ++i)
        {
            fragFile.seekp(numberPosFrag + i);
            fragFile.put( numberString[i] );
        }

        fragFile.close();
        vertFile.close();
    }
    s_envMap = EnvironmentTexture("textures/HDRI/Factory_Catwalk_2k.hdr");

    ngl::ShaderLib::useNullProgram();

    ngl::ShaderLib::loadShader(m_name, m_vertPath, m_fragPath);
    RefreshCurrentLights();
}

void PBRShaderManager::RefreshCurrentLights()
{
    ngl::ShaderLib::use(m_name);
    for(int i = 0; i < s_directionalLights.size(); ++i)
    {
        ngl::ShaderLib::setUniform(("dirLightDirs[" + std::to_string(i) + "]").c_str(),
            s_directionalLights[i]->GetForward() );
        ngl::ShaderLib::setUniform(("dirLightColors[" + std::to_string(i) + "]").c_str(),
            s_directionalLights[i]->GetColor());
        ngl::ShaderLib::setUniform(("dirLightIntensities[" + std::to_string(i) + "]").c_str(),
            s_directionalLights[i]->GetIntensity());
    }

    for(int i = 0; i < s_pointLights.size(); ++i)
    {
        /*ngl::ShaderLib::setUniform(("pLightDirs[" + std::to_string(i) + "]").c_str(),
            pl[i].GetForward() );*/
        ngl::ShaderLib::setUniform(("pLightColors[" + std::to_string(i) + "]").c_str(),
            s_pointLights[i]->GetColor());

        ngl::ShaderLib::setUniform(("pLightIntensities[" + std::to_string(i) + "]").c_str(),
            s_pointLights[i]->GetIntensity());
        ngl::ShaderLib::setUniform(("pLightPos[" + std::to_string(i) + "]").c_str(),
            s_pointLights[i]->GetPosition());
    }
}

void PBRShaderManager::UseShader()
{
    ngl::ShaderLib::use(m_name);
}

std::shared_ptr<Light> PBRShaderManager::AddDirectionalLight(const ngl::Vec3& position, const ngl::Vec3& rotation, const ngl::Vec3& color, float intensity)
{
    std::shared_ptr<Light> light = std::make_shared<Light>(LightType::Directional, position, rotation, color, intensity);
    s_directionalLights.push_back(light);
    UpdateLightCounts();
    return light;
}

std::shared_ptr<Light> PBRShaderManager::AddPointLight(const ngl::Vec3& position, const ngl::Vec3& color, float intensity)
{
    std::shared_ptr<Light> light = std::make_shared<Light>(LightType::Point, position, ngl::Vec3(0,0,0), color, intensity);
    s_pointLights.push_back(light);
    UpdateLightCounts();
    return light;
}

void PBRShaderManager::RemoveDirectionalLight( std::shared_ptr<Light> light )
{
    s_directionalLights.erase( std::find(s_directionalLights.begin(), s_directionalLights.end(), light) );
    PBRShaderManager::s_curDirShadowIndex = 0;
    for(int i = 0; i < s_directionalLights.size() && i < PBRShaderManager::s_maxDirectionalShadows; i++)
    {
        s_directionalLights[i]->RecalculateShadowBuffer(i);
    }
    UpdateLightCounts();
}

void PBRShaderManager::RemovePointLight( std::shared_ptr<Light> light )
{
    s_pointLights.erase( std::find(s_pointLights.begin(), s_pointLights.end(), light) );

    UpdateLightCounts();
}