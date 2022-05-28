#include <EnvironmentTexture.h>
#include "Assets/AssetManager.h"
#include "ObjMesh.h"
#include "ngl/Util.h"

EnvironmentTexture::EnvironmentTexture(const std::string& path)
{
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
        /*for(int i = 0; i < width*height*nrComponents; i+=3)
        {
            //std::cout<<data[i]<<" "<<data[i+1]<<" "<<data[i+2]<<"\n";
        }*/

        stbi_image_free(data); 
    }
    else
    {
        std::cout << "Failed to load HDR image." << std::endl;
    }

    /*FramebufferSpecification fbSpec;
	fbSpec.Attachments = { FramebufferTextureFormat::RGB16F, FramebufferTextureFormat::Depth };
	fbSpec.Width = m_textureSize;
	fbSpec.Height = m_textureSize;
	m_framebuffer = std::make_unique<FrameBuffer>(fbSpec);*/

    unsigned int captureFBO, captureRBO;
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);

    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_textureSize, m_textureSize);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);  

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
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    m_mesh = AssetManager::GetAsset<ObjMesh>("meshes/UnitCube.obj");

    m_projection = ngl::perspective(90.0f, 1.0f, 0.1f, 10.0f);
    std::array<ngl::Mat4, 6> captureViews;
    ngl::Vec3 position = ngl::Vec3(0,0,0);
    captureViews[0] = m_projection * ngl::lookAt( position, ngl::Vec3( 1, 0, 0 ), ngl::Vec3(0, -1, 0) );
    captureViews[1] = m_projection * ngl::lookAt( position, ngl::Vec3( -1, 0, 0 ), ngl::Vec3(0, -1, 0) );
    captureViews[2] = m_projection * ngl::lookAt( position, ngl::Vec3( 0, 1, 0 ), ngl::Vec3(0, 0, 1) );
    captureViews[3] = m_projection * ngl::lookAt( position, ngl::Vec3( 0, -1, 0 ), ngl::Vec3(0, 0, -1) );
    captureViews[4] = m_projection * ngl::lookAt( position, ngl::Vec3( 0, 0, 1 ), ngl::Vec3(0, -1, 0) );
    captureViews[5] = m_projection * ngl::lookAt( position, ngl::Vec3( 0, 0, -1 ), ngl::Vec3(0, -1, 0) );

    ngl::ShaderLib::use("HDRToCubemap");
    //ngl::ShaderLib::setUniform("projection", m_projection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_hdrID);
    glViewport(0, 0, m_textureSize, m_textureSize);
    //m_framebuffer->Bind();
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for (unsigned int i = 0; i < 6; ++i)
    {
        ngl::ShaderLib::setUniform("VP", captureViews[i]);
        //m_framebuffer->SetAttachment(GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_cubemapID);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
                           GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_cubemapID, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_mesh->Draw(); // renders a 1x1 cube
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0); 
    //m_framebuffer->Unbind();
}