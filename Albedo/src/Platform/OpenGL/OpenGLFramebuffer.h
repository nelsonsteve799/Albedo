#pragma once

#include "Albedo/Renderer/Framebuffer.h"

namespace Albedo {
	class OpenGLFramebuffer : public Framebuffer
	{
	public:
		OpenGLFramebuffer(const FramebufferSpecification& spec, const TextureConfiguration& config = TextureConfiguration());
		virtual ~OpenGLFramebuffer();

		void Invalidate();

		virtual void Bind() override;
		virtual void Unbind() override;

		virtual void Resize(uint32_t width, uint32_t height) override;

		virtual void ClearAttachment(uint32_t attachmentIndex, int value) override;

		virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) override;

		virtual uint32_t GetFramebufferID() const override { return m_FramebufferID; }
		virtual uint32_t GetRenderbufferID() const override { return m_RenderbufferAttachment; }

		virtual uint32_t GetDepthAttachmentID() const override { return m_DepthAttachment; }
		virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const override { Albedo_CORE_ASSERT(index < m_ColorAttachments.size(), "index out of range"); return m_ColorAttachments[index]; }

		virtual const FramebufferSpecification& GetSpecification() const override { return m_Specification; }
	private:
		uint32_t m_FramebufferID = 0;
		FramebufferSpecification m_Specification;

		std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecifications;
		FramebufferTextureSpecification m_DepthAttachmentSpecification = FramebufferTextureFormat::None;
		FramebufferTextureSpecification m_RenderbufferAttachmentSpecification = FramebufferTextureFormat::None;

		TextureConfiguration m_TextureConfiguration;

		std::vector<uint32_t> m_ColorAttachments;
		uint32_t m_DepthAttachment = 0;
		uint32_t m_RenderbufferAttachment = 0;
	};

}