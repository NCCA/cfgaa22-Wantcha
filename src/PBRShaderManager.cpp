#include "PBRShaderManager.h"
#include <fstream>

uint32_t PBRShaderManager::s_whiteTextureID = 0;

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
	glTextureStorage2D(s_whiteTextureID, 1, GL_RGBA8, 2, 2);

	glTextureParameteri(s_whiteTextureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(s_whiteTextureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTextureParameteri(s_whiteTextureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(s_whiteTextureID, GL_TEXTURE_WRAP_T, GL_REPEAT);


    uint32_t whiteTextureData = 0xffffffff;
    glTextureSubImage2D(s_whiteTextureID, 0, 0, 0, 2, 2, GL_RGBA, GL_UNSIGNED_BYTE, &whiteTextureData);
}

std::string PBRShaderManager::m_fragPath = "";
std::string PBRShaderManager::m_vertPath = "";
std::string PBRShaderManager::m_name = "";

void PBRShaderManager::Init(const std::string& name,
            const std::string& vert, const std::string& frag)
{
    m_name = name;
    m_vertPath = vert;
    m_fragPath = frag;
    CreateWhiteTexture();
}

void PBRShaderManager::UpdateLightCounts(std::vector<Light>& dl, std::vector<Light>& pl)
{
    // Dynamic light count
    std::fstream file;
    file.open(m_fragPath, std::ios::in| std::ios::out | std::ios::binary );
    std::string fragText;

    const std::string dirCountText = "dirLightCount";
    const std::string pointCountText = "pLightCount";

    if (file)
    {
        file.seekg(0, std::ios::end);
        fragText.resize(file.tellg());
        file.seekg(0, std::ios::beg);
        file.read(&fragText[0], fragText.size());

        //Directional lights
        size_t pos; 
        pos = fragText.find(dirCountText);

        size_t numberPos = pos + dirCountText.length() + 3;
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

        numberPos = pos + pointCountText.length() + 3;
        numberString = std::to_string( static_cast<int>(pl.size()) );
        for(int i = 0; i < numberString.length(); ++i)
        {
            file.seekp(numberPos + i);
            file.put( numberString[i] );
        }

        file.close();
    }

    ngl::ShaderLib::useNullProgram();

    ngl::ShaderLib::loadShader(m_name, m_vertPath, m_fragPath);
    ngl::ShaderLib::use(m_name);

    for(int i = 0; i < dl.size(); ++i)
    {
        ngl::ShaderLib::setUniform(("dirLightDirs[" + std::to_string(i) + "]").c_str(),
            dl[i].GetForward() );
        ngl::ShaderLib::setUniform(("dirLightColors[" + std::to_string(i) + "]").c_str(),
            dl[i].GetColor());
        ngl::ShaderLib::setUniform(("dirLightIntensities[" + std::to_string(i) + "]").c_str(),
            dl[i].GetIntensity());
    }
}

void PBRShaderManager::UseShader()
{
    ngl::ShaderLib::use(m_name);
}