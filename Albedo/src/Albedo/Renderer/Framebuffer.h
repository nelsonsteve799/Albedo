#pragma once

#include "Albedo/Core/Core.h"
#include "Texture.h"

namespace Albedo {

	enum class FramebufferTextureFormat
	{
		None = 0,
		// Color
		RGBA8,
		RED_INTEGER,
		// Depth/stencil
		DEPTH24STENCIL8,
		Depth,
		//Renderbuffer
		RENDER_BUFFER
	};

	struct FramebufferTextureSpecification
	{
		FramebufferTextureSpecification() = default;
		FramebufferTextureSpecification(FramebufferTextureFormat format)
			: TextureFormat(format) {}

		FramebufferTextureFormat TextureFormat = FramebufferTextureFormat::None;
		// TODO: filtering/wrap
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
		TextureConfiguration config;

		uint32_t Width = 0, Height = 0;
		FramebufferAttachmentSpecification Attachments;
		uint32_t Samples = 1;

		bool s_DrawBuffer = true;
		bool s_ReadBuffer = true;
		bool s_BorderColor   = false;
		bool SwapChainTarget = false;
	};

	class Framebuffer
	{
	public:
		virtual ~Framebuffer() = default;
		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		//virtual uint32_t GetRendererID() const = 0;

		virtual void ClearAttachment(uint32_t attachmentIndex, int value) = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;

		virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) = 0;

		virtual uint32_t GetFramebufferID() const = 0;
		virtual uint32_t GetRenderbufferID() const = 0;

		virtual uint32_t GetDepthAttachmentID() const = 0;
		virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const = 0;

		virtual const FramebufferSpecification& GetSpecification() const = 0;

		static Ref<Framebuffer> Create(const FramebufferSpecification& spec);
		static Ref<Framebuffer> Create(const FramebufferSpecification& spec, TextureConfiguration config);
	};


}