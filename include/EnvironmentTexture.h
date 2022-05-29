#ifndef ENVIRONMENT_TEXTURE_H
#define ENVIRONMENT_TEXTURE_H

#include <ngl/Vec3.h>
#include <ngl/Mat4.h>
#include <FrameBuffer.h>
#include <string>
#include <ngl/AbstractVAO.h>
#include <ngl/VAOFactory.h>

class Mesh;

class EnvironmentTexture
{
public:
    EnvironmentTexture() = default;
    EnvironmentTexture( uint32_t textureID );
    //EnvironmentTexture( const std::string& path );
    
    ~EnvironmentTexture();
    std::shared_ptr<Mesh> GetCube() { return m_mesh; }
    uint32_t GetIradianceMap() const { return m_iradianceID; }
    uint32_t GetEnvironmentCubeMap() const { return m_cubemapID; }
    uint32_t GetBRDFMap() const { return m_brdfLUTID; }
    uint32_t GetPrefilteredMap() const { return m_prefilterID; }
    uint32_t GetHDRMap() const { return m_hdrID; }
    uint32_t* GetHDRMapPointer() { return &m_hdrID; }
    std::string GetFilepath() const { return m_filepath; }

    void SetTexture(const std::string& path);
    void SetTexture(uint32_t textureID);

private:
    void Render();

    uint32_t m_hdrID = 0;
    uint32_t m_cubemapID = 0;
    uint32_t m_iradianceID = 0;
    uint32_t m_prefilterID = 0;
    uint32_t m_brdfLUTID = 0;

    uint32_t m_textureSize = 512;
    uint32_t m_iradianceSize = 64;
    uint32_t m_prefilterSize = 256;

    uint32_t m_captureFBO = 0, m_captureRBO = 0;
    std::shared_ptr<Mesh> m_mesh;
    std::unique_ptr<ngl::AbstractVAO> m_quad;
    std::array<ngl::Mat4, 6> m_captureViews;
    std::string m_filepath;
};
#endif