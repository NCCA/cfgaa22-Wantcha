#ifndef PBR_SHADER_MANAGER_H
#define PBR_SHADER_MANAGER_H

#include <ngl/ShaderLib.h>
#include <string>
#include <vector>
#include <Lights.h>

class PBRShaderManager
{

public:

    PBRShaderManager() = default;
    ~PBRShaderManager() = default;

    PBRShaderManager (const std::string& name, const std::string& vert, const std::string& frag);

    void UpdateLightCounts(const std::vector<DirectionalLight>& dl,
                            const std::vector<PointLight>& pl);

private:
    std::string m_fragPath;
    std::string m_vertPath;
    std::string m_name;


};

#endif