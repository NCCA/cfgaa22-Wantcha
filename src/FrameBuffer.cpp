#include "FrameBuffer.h"
#include <ngl/Shader.h>

static const uint32_t s_MaxFrameBufferSize = 8192;

// Utility functions
namespace Utils
{
    static GLenum TextureTarget(bool multisampled)
    {
        return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
    }

    static void CreateTextures(bool multisampled, uint32_t* outID, uint32_t count)
    {
        glCreateTextures(TextureTarget(multisampled), count, outID);
    }

    static void BindTexture(bool multisampled, uint32_t id)
    {
        glBindTexture(TextureTarget(multisampled), id);
    }

    static void AttachColorTexture(uint32_t id, int samples, GLenum internalFormat, GLenum format, uint32_t width, uint32_t height, int index)
    {
        bool multisampled = samples > 1;
        if (multisampled)
        {
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_FALSE);
        }
        else
        {
            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(multisampled), id, 0);
    }

    static void AttachDepthTexture(uint32_t id, int samples, GLenum format, GLenum attachmentType, uint32_t width, uint32_t height)
    {
        bool multisampled = samples > 1;
        if (multisampled)
        {
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
        }
        else
        {
            glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, TextureTarget(multisampled), id, 0);
    }

    static bool IsDepthFormat(FramebufferTextureFormat format)
    {
        switch (format)
        {
            case FramebufferTextureFormat::DEPTH24STENCIL8: return true; break;
        }

        return false;
    }

    static GLenum FBTextureFormatToGL(FramebufferTextureFormat format)
    {
        switch (format)
        {
            case FramebufferTextureFormat::RGBA8: return GL_RGBA8;
            case FramebufferTextureFormat::RED_INTEGER: return GL_RED_INTEGER;
            case FramebufferTextureFormat::DEPTH24STENCIL8: return GL_DEPTH24_STENCIL8;
        }
        return 0;
    }
}

FrameBuffer::FrameBuffer(const FramebufferSpecification& spec)
    : m_Specs(spec)
{
    for (auto format : m_Specs.Attachments.Attachments)
    {
        if (!Utils::IsDepthFormat(format.TextureFormat))
            m_ColorAttachmentSpecs.emplace_back(format);
        else
            m_DepthAttachmentSpec = format;
    }

    Invalidate();
}

FrameBuffer::~FrameBuffer()
{
    glDeleteFramebuffers(1, &m_RendererID);
    glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
    glDeleteTextures(1, &m_DepthAttachment);
}

void FrameBuffer::Invalidate()
{
    if (m_RendererID)
    {
        glDeleteFramebuffers(1, &m_RendererID);
        glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
        glDeleteTextures(1, &m_DepthAttachment);

        m_ColorAttachments.clear();
        m_DepthAttachment = 0;
    }

    glCreateFramebuffers(1, &m_RendererID);
    glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

    bool multisample = m_Specs.Samples > 1;

    //Attachments
    if (m_ColorAttachmentSpecs.size())
    {
        m_ColorAttachments.resize(m_ColorAttachmentSpecs.size());
        Utils::CreateTextures(multisample, m_ColorAttachments.data(), m_ColorAttachments.size());

        for (size_t i = 0; i < m_ColorAttachments.size(); i++)
        {
            Utils::BindTexture(multisample, m_ColorAttachments[i]);
            switch (m_ColorAttachmentSpecs[i].TextureFormat)
            {
            case FramebufferTextureFormat::RGBA8:
                Utils::AttachColorTexture(m_ColorAttachments[i], m_Specs.Samples, GL_RGBA8, GL_RGBA, m_Specs.Width, m_Specs.Height, i);
                break;
            case FramebufferTextureFormat::RED_INTEGER:
                Utils::AttachColorTexture(m_ColorAttachments[i], m_Specs.Samples, GL_R32I, GL_RED_INTEGER, m_Specs.Width, m_Specs.Height, i);
                break;
            }
        }
    }

    if (m_DepthAttachmentSpec.TextureFormat != FramebufferTextureFormat::None)
    {
        Utils::CreateTextures(multisample, &m_DepthAttachment, 1);
        Utils::BindTexture(multisample, m_DepthAttachment);
        switch (m_DepthAttachmentSpec.TextureFormat)
        {
        case FramebufferTextureFormat::DEPTH24STENCIL8:
            Utils::AttachDepthTexture(m_DepthAttachment, m_Specs.Samples, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, m_Specs.Width, m_Specs.Height);
            break;
        }
    }

    if (m_ColorAttachments.size() > 1)
    {
        GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
        glDrawBuffers(m_ColorAttachments.size(), buffers);
    }
    else if (m_ColorAttachments.empty())
    {
        // Only depth pass
        glDrawBuffer(GL_NONE);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void FrameBuffer::Bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_RendererID);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_RendererID);
    glViewport(0, 0, m_Specs.Width, m_Specs.Height);
}

void FrameBuffer::Unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::BlitToScreen()
{
    //std::cout<<"AA";
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_RendererID);

    glBlitFramebuffer(0, 0, m_Specs.Width, m_Specs.Height,
                      0, 0, m_Specs.Width, m_Specs.Height,
                      GL_COLOR_BUFFER_BIT,
                      GL_NEAREST);
    
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void FrameBuffer::Resize(uint32_t width, uint32_t height)
{
    if (width == 0 || height == 0 || width > s_MaxFrameBufferSize || height > s_MaxFrameBufferSize)
    {
        std::cout<<"TRYING TO RESIZE FRAMEBUFFER TO 0!\n";
        return;
    }

    m_Specs.Width = width;
    m_Specs.Height = height;
    Invalidate();
}

int FrameBuffer::ReadPixel(uint32_t attachmentIndex, int x, int y)
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_RendererID);

    glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
    int pixelData;
    glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);

    glReadBuffer(GL_COLOR_ATTACHMENT0);
    //glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    //glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    //Bind();

    return pixelData;
}

void FrameBuffer::ClearAttachment(uint32_t attachmentIndex, int value)
{
    auto& spec = m_ColorAttachmentSpecs[attachmentIndex];

    glClearTexImage(m_ColorAttachments[attachmentIndex], 0, Utils::FBTextureFormatToGL(spec.TextureFormat), GL_INT, &value);
}