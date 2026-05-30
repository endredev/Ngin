#include "ngpch.h"
#include "DX12RendererSubsystem.h"
#include "DX12Context.h"
#include "Ngin/Log.h"
#include "Ngin/Renderer/Renderer.h"
#include "Ngin/Renderer/Renderer2D.h"
#include "Ngin/Renderer/RenderCommand.h"
#include "Platforms/Windows/WindowSubsystem.h"
#include "Platforms/Windows/WindowsWindow.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

namespace Ngin {

DX12RendererSubsystem::DX12RendererSubsystem(WindowSubsystem* windowSys)
	: m_WindowSys(windowSys)
{}

void DX12RendererSubsystem::OnInit()
{
	auto* win  = static_cast<WindowsWindow*>(m_WindowSys->GetWindow());
	HWND  hwnd = win->GetHWND();

	DX12Context::Init(hwnd, m_WindowSys->GetWidth(), m_WindowSys->GetHeight());

	m_ViewportFB = std::make_unique<DX12Framebuffer>(1280, 720);

	Renderer::Init();
	Renderer2D::Init();

	NG_CORE_INFO("DX12RendererSubsystem initialized.");
}

void DX12RendererSubsystem::OnShutdown()
{
	Renderer2D::Shutdown();
	Renderer::Shutdown();
	m_ViewportFB.reset();
	DX12Context::Shutdown();
	NG_CORE_INFO("DX12RendererSubsystem shutdown.");
}

void DX12RendererSubsystem::BeginFrame()
{
	// Apply pending resize BEFORE the command list opens
	if (m_PendingW > 0 &&
	    (m_PendingW != m_ViewportFB->GetWidth() ||
	     m_PendingH != m_ViewportFB->GetHeight()))
	{
		DX12Context::Get().WaitIdle();
		m_ViewportFB->Resize(m_PendingW, m_PendingH);
	}

	DX12Context::Get().BeginFrame();
	m_ViewportFB->Bind();
	RenderCommand::SetClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	RenderCommand::Clear();
}

void DX12RendererSubsystem::EndFrame()
{
	m_ViewportFB->Unbind();
}

} // namespace Ngin
