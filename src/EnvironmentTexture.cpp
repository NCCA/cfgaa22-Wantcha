#include <EnvironmentTexture.h>
#include "Assets/AssetManager.h"
#include "ObjMesh.h"
#include "ngl/Util.h"
#include <iostream>
#include <ngl/SimpleVAO.h>

#ifndef _WIN32
#define STB_IMAGE_IMPLEMENTATION
#endif
#include <../3rdparty/stb_image.h>

EnvironmentTexture::EnvironmentTexture(uint32_t textureID)
{
    struct SimpleVertData
    {
        ngl::Vec3 pos;
        ngl::Vec2 uv;
    };

    ngl::Vec3 points[4] = { ngl::Vec3{ -1, -1, 0 }, ngl::Vec3{ 1, -1, 0 }, ngl::Vec3{ 1, 1, 0 }, ngl::Vec3{ -1, 1, 0 } };
    ngl::Vec2 uvs[4] = { ngl::Vec2{ 0,0 }, ngl::Vec2{ 1, 0 }, ngl::Vec2{ 1,1 }, ngl::Vec2{ 0,1 } };

    SimpleVertData vbo[6];
    vbo[0].pos = points[0]; vbo[0].uv = uvs[0];
    vbo[1].pos = points[1]; vbo[1].uv = uvs[1];
    vbo[2].pos = points[2]; vbo[2].uv = uvs[2];
    vbo[3].pos = points[3]; vbo[3].uv = uvs[3];
    vbo[4].pos = points[0]; vbo[4].uv = uvs[0];
    vbo[5].pos = points[2]; vbo[5].uv = uvs[2];

    m_quad = ngl::VAOFactory::createVAO(ngl::simpleVAO, GL_TRIANGLES);
    m_quad->bind();

    m_quad->setData(ngl::SimpleVAO::VertexData(6*sizeof(SimpleVertData), vbo[0].pos.m_x));
    m_quad->setVertexAttributePointer(0, 3, GL_FLOAT, sizeof(SimpleVertData), 0);
    m_quad->setVertexAttributePointer(1, 2, GL_FLOAT, sizeof(SimpleVertData), 3);
    m_quad->setNumIndices(6);

    m_quad->unbind();

    glGenFramebuffers(1, &m_captureFBO);
    glGenRenderbuffers(1, &m_captureRBO);

    m_mesh = AssetManager::GetAsset<ObjMesh>("meshes/UnitCube.obj");

    ngl::Mat4 projection = ngl::perspective(90.0f, 1.0f, 0.1f, 10.0f);
    ngl::Vec3 position = ngl::Vec3(0,0,0);
    m_captureViews[0] = projection * ngl::lookAt( position, ngl::Vec3( 1, 0, 0 ), ngl::Vec3(0, -1, 0) );
    m_captureViews[1] = projection * ngl::lookAt( position, ngl::Vec3( -1, 0, 0 ), ngl::Vec3(0, -1, 0) );
    m_captureViews[2] = projection * ngl::lookAt( position, ngl::Vec3( 0, 1, 0 ), ngl::Vec3(0, 0, 1) );
    m_captureViews[3] = projection * ngl::lookAt( position, ngl::Vec3( 0, -1, 0 ), ngl::Vec3(0, 0, -1) );
    m_captureViews[4] = projection * ngl::lookAt( position, ngl::Vec3( 0, 0, 1 ), ngl::Vec3(0, -1, 0) );
    m_captureViews[5] = projection * ngl::lookAt( position, ngl::Vec3( 0, 0, -1 ), ngl::Vec3(0, -1, 0) );

    glGenTextures(1, &m_brdfLUTID);
    glBindTexture(GL_TEXTURE_2D, m_brdfLUTID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    glBindFramebuffer(GL_FRAMEBUFFER, m_captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, m_captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_brdfLUTID, 0);

    glViewport(0, 0, 512, 512);
    ngl::ShaderLib::use("BRDF");
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_quad->bind();
    m_quad->draw();
    m_quad->bind();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);  

    SetTexture(textureID);
}

void EnvironmentTexture::SetTexture(const std::string& path)
{
    if(m_hdrID && m_hdrID != PBRShaderManager::s_whiteTextureID)
    {
        glDeleteTextures(1, &m_hdrID);
    }
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrComponents;
    float *data = stbi_loadf(path.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        glGenTextures(1, &m_hdrID);
        glBindTexture(GL_TEXTURE_2D, m_hdrID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data); 

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data); 
        m_filepath = path;
    }
    else
    {
        std::cout << "Failed to load HDR image." << std::endl;
        SetTexture(PBRShaderManager::s_whiteTextureID);
    }
    Render();
}

void EnvironmentTexture::SetTexture(uint32_t textureID)
{
    if(m_hdrID)
    {
        glDeleteTextures(1, &m_hdrID);
    }
    m_hdrID = textureID;
    m_filepath = "";
    Render();
}

void EnvironmentTexture::Render()
{
    if(m_cubemapID)
    {
        glDeleteTextures(1, &m_cubemapID);
    }
    glGenTextures(1, &m_cubemapID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemapID);
    for (unsigned int i = 0; i < 6; ++i)
    {
        // note that we store each face with 16 bit floating point values
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 
                    m_textureSize, m_textureSize, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if(m_iradianceID)
    {
        glDeleteTextures(1, &m_iradianceID);
    }
    glGenTextures(1, &m_iradianceID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_iradianceID);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, m_iradianceSize, m_iradianceSize, 0, 
                    GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if(m_prefilterID)
    {
        glDeleteTextures(1, &m_prefilterID);
    }
    glGenTextures(1, &m_prefilterID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_prefilterID);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, m_prefilterSize, m_prefilterSize, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    glBindFramebuffer(GL_FRAMEBUFFER, m_captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, m_captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_textureSize, m_textureSize);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_captureRBO);

    ngl::ShaderLib::use("HDRToCubemap");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_hdrID);
    glViewport(0, 0, m_textureSize, m_textureSize);
    //m_framebuffer->Bind();
    glBindFramebuffer(GL_FRAMEBUFFER, m_captureFBO);
    for (unsigned int i = 0; i < 6; ++i)
    {
        ngl::ShaderLib::setUniform("VP", m_captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
                           GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_cubemapID, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_mesh->Draw(); // renders a 1x1 cube
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemapID);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, m_captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, m_captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_iradianceSize, m_iradianceSize);

    ngl::ShaderLib::use("Iradiance");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemapID);
    glViewport(0, 0, m_iradianceSize, m_iradianceSize);
    glBindFramebuffer(GL_FRAMEBUFFER, m_captureFBO);
    for (unsigned int i = 0; i < 6; ++i)
    {
        ngl::ShaderLib::setUniform("VP", m_captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
                           GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_iradianceID, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_mesh->Draw();
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    ngl::ShaderLib::use("PrefilterHDR");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemapID);
    glBindFramebuffer(GL_FRAMEBUFFER, m_captureFBO);
    
    unsigned int maxMipLevels = 5;
    for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
    {
        // reisze framebuffer according to mip-level size.
        unsigned int mipWidth  = m_prefilterSize * std::pow(0.5, mip);
        unsigned int mipHeight = mipWidth;
        glBindRenderbuffer(GL_RENDERBUFFER, m_captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
        glViewport(0, 0, mipWidth, mipHeight);

        float roughness = (float)mip / (float)(maxMipLevels - 1);
        ngl::ShaderLib::setUniform("roughness", roughness);
        ngl::ShaderLib::setUniform("resolution", static_cast<float>(m_textureSize) );
        for(unsigned int i = 0; i < 6; ++i)
        {
            ngl::ShaderLib::setUniform("VP", m_captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
                            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_prefilterID, mip);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            m_mesh->Draw();
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

EnvironmentTexture::~EnvironmentTexture()
{
    /*glDeleteTextures(1, &m_hdrID);
    glDeleteTextures(1, &m_cubemapID);
    glDeleteTextures(1, &m_iradianceID);*/
}