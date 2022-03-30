#include "PBRShaderManager.h"
#include <fstream>
PBRShaderManager::PBRShaderManager(const std::string& name, const std::string& vert, const std::string& frag)
    : m_vertPath(vert), m_fragPath(frag), m_name(name)
{

}

void PBRShaderManager::UpdateLightCounts(const std::vector<DirectionalLight>& dl, const std::vector<PointLight>& pl)
{
    // Dynamic light count
    std::fstream file;
    file.open(m_fragPath, std::ios::in| std::ios::out );
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
        const char* numberChar = std::to_string( dl.size() ).c_str();
        for(int i = 0; i < strlen(numberChar); ++i)
        {
            file.seekp(numberPos + i);
            file.put( numberChar[i] );
        }

        // Point lights
        file.seekg(0, std::ios::beg);
        pos = fragText.find(pointCountText);

        numberPos = pos + pointCountText.length() + 3;
        numberChar = std::to_string( pl.size() ).c_str();
        for(int i = 0; i < strlen(numberChar); ++i)
        {
            file.seekp(numberPos + i);
            file.put( numberChar[i] );
        }


        file.close();
    }

    ngl::ShaderLib::useNullProgram();

    ngl::ShaderLib::loadShader(m_name, m_vertPath, m_fragPath);
    ngl::ShaderLib::use(m_name);

    for(int i = 0; i < dl.size(); ++i)
    {
        ngl::ShaderLib::setUniform(("dirLightDirs[" + std::to_string(i) + "]").c_str(),
            dl[i].GetDirection());
        ngl::ShaderLib::setUniform(("dirLightColors[" + std::to_string(i) + "]").c_str(),
            dl[i].GetColor());
        ngl::ShaderLib::setUniform(("dirLightIntensities[" + std::to_string(i) + "]").c_str(),
            dl[i].GetIntensity());
    }
}