#pragma once

#include "Ngin/Renderer/Framebuffer.h"

namespace Ngin {

	class OpenGLFramebuffer : public Framebuffer
	{
	public:
		OpenGLFramebuffer(uint32_t width, uint32_t height);
		~OpenGLFramebuffer();

		void Bind() const override;
		void Unbind() const override;
		void Resize(uint32_t width, uint32_t height) override;

		uint32_t GetColorAttachmentID() const override { return m_ColorAttachment; }
		uint32_t GetWidth()  const override { return m_Width; }
		uint32_t GetHeight() const override { return m_Height; }

	private:
		void Invalidate();

		uint32_t m_RendererID      = 0;
		uint32_t m_ColorAttachment = 0;
		uint32_t m_DepthAttachment = 0;
		uint32_t m_Width, m_Height;
	};

}
