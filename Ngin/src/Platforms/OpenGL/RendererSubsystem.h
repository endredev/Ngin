#pragma once

#include "Ngin/Subsystem.h"
#include "Ngin/Renderer/Framebuffer.h"
#include "Ngin/Renderer/RenderCommand.h"

#include <memory>

namespace Ngin {

	class NG_API RendererSubsystem : public Subsystem
	{
	public:
		void OnInit() override;
		void OnShutdown() override;

		void BeginFrame()
		{
			m_ViewportFB->Bind();
			RenderCommand::SetClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			RenderCommand::Clear();
		}

		void EndFrame() { m_ViewportFB->Unbind(); }

		uint32_t GetViewportTextureID() const { return m_ViewportFB->GetColorAttachmentID(); }
		void ResizeViewport(uint32_t width, uint32_t height) { m_ViewportFB->Resize(width, height); }

	private:
		std::unique_ptr<Framebuffer> m_ViewportFB;
	};

}
