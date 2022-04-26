#ifndef FRAME_BUFFER_H
#define FRAME_BUFFER_H

/// Code adapted from my older game engine project
/// https://github.com/Wantcha/Chimera/blob/main/Chimera/src/Platform/OpenGL/OpenGLFramebuffer.h

#include <memory>
#include <cstdint>
#include <vector>

enum class FramebufferTextureFormat
	{
		None = 0,

		// Color
		RGBA8,
		RED_INTEGER,

		// Depth/stencil
		DEPTH24STENCIL8,

		// Defaults
		Depth = DEPTH24STENCIL8
	};

	struct FramebufferTextureSpecification
	{
		FramebufferTextureSpecification() = default;
		FramebufferTextureSpecification(FramebufferTextureFormat format)
			: TextureFormat(format) {}

		FramebufferTextureFormat TextureFormat = FramebufferTextureFormat::None;
	};

	struct FramebufferAttachmentSpecification
	{
		FramebufferAttachmentSpecification() = default;
		FramebufferAttachmentSpecification(std::initializer_list<FramebufferTextureSpecification> attachments)
			: Attachments(attachments) {}

		std::vector<FramebufferTextureSpecification> Attachments;
	};

	struct FramebufferSpecification
	{
		uint32_t Width = 0, Height = 0;
		FramebufferAttachmentSpecification Attachments;
		//FramebufferFormat Format = 
		uint32_t Samples = 1;

		bool SwapChainTarget = false;
		
	};

class FrameBuffer
{
public:
    FrameBuffer(const FramebufferSpecification& spec);
    virtual ~FrameBuffer();

    void Invalidate();

    void Bind();
    void Unbind();

    void BlitToScreen();

    void Resize(uint32_t width, uint32_t height);
    int ReadPixel(uint32_t attachmentIndex, int x, int y);

    void ClearAttachment(uint32_t attachmentIndex, int value);

    uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const { return m_ColorAttachments[index]; }

    const FramebufferSpecification& GetSpecification() const { return m_Specs; };
private:
    uint32_t m_RendererID = 0;
    FramebufferSpecification m_Specs;

    std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecs;
    FramebufferTextureSpecification m_DepthAttachmentSpec = FramebufferTextureFormat::None;

    std::vector<uint32_t> m_ColorAttachments;
    uint32_t m_DepthAttachment = 0;
};

#endif