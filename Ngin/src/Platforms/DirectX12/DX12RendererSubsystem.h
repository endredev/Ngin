#pragma once

#include "Ngin/Subsystem.h"
#include "DX12Framebuffer.h"

#include <memory>

namespace Ngin {

class WindowSubsystem;

class NG_API DX12RendererSubsystem : public Subsystem
{
public:
	explicit DX12RendererSubsystem(WindowSubsystem* windowSys);

	void OnInit()     override;
	void OnShutdown() override;

	void BeginFrame();
	void EndFrame();

	uint64_t GetViewportTextureID() const { return m_ViewportFB->GetColorAttachmentID(); }
	void ResizeViewport(uint32_t width, uint32_t height)
	{
		if (width > 0 && height > 0) { m_PendingW = width; m_PendingH = height; }
	}

private:
	WindowSubsystem*                 m_WindowSys;
	std::unique_ptr<DX12Framebuffer> m_ViewportFB;
	uint32_t                         m_PendingW = 0;
	uint32_t                         m_PendingH = 0;
};

} // namespace Ngin
