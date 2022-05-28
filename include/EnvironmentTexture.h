#ifndef ENVIRONMENT_TEXTURE_H
#define ENVIRONMENT_TEXTURE_H

#include <../3rdparty/stb_image.h>
#include <iostream>
#include <ngl/Vec3.h>
#include <ngl/Mat4.h>
#include <FrameBuffer.h>
#include <string>

class Mesh;
class EnvironmentTexture
{
public:
    EnvironmentTexture() = default;
    EnvironmentTexture( const std::string& path );
    std::shared_ptr<Mesh> GetCube() { return m_mesh; }
    uint32_t GetEnvironmentMap() const { return m_cubemapID; }

private:

    std::unique_ptr<FrameBuffer> m_framebuffer;
    uint32_t m_hdrID = 0;
    uint32_t m_cubemapID = 0;
    uint32_t m_textureSize = 512;
    std::shared_ptr<Mesh> m_mesh;
    ngl::Mat4 m_projection;
};
#endif