#include "ngpch.h"

#include "Application.h"
#include "Ngin/Log.h"
#include "Ngin/EditorStyle.h"
#include "Ngin/Renderer/Renderer.h"
#include "Ngin/Renderer/PerspectiveCamera.h"
#include "Platforms/Windows/WindowSubsystem.h"
#include "Platforms/Windows/InputSubsystem.h"
#include "Platforms/DirectX12/DX12RendererSubsystem.h"
#include "Platforms/DirectX12/DX12ImGuiSubsystem.h"

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_internal.h>

namespace Ngin {

Application::Application()
{
	auto* windowSys = m_SubsystemManager.Register<WindowSubsystem>();
	windowSys->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));

	m_SubsystemManager.Register<DX12RendererSubsystem>(windowSys);
	m_SubsystemManager.Register<InputSubsystem>();
	m_SubsystemManager.Register<DX12ImGuiSubsystem>(windowSys);

	m_SubsystemManager.InitAll();

	m_SceneCamera = std::make_unique<PerspectiveCamera>(
		0.785398f,  // 45 deg fov
		16.0f / 9.0f,
		0.1f, 1000.0f
	);
	static_cast<PerspectiveCamera*>(m_SceneCamera.get())->SetPosition(0.0f, 0.0f, 3.0f);
}

Application::~Application()
{
	m_SubsystemManager.ShutdownAll();
}

void Application::Run()
{
	EditorStyle::Apply();

	float lastTime = (float)glfwGetTime();

	auto* renderer = m_SubsystemManager.Get<DX12RendererSubsystem>();
	auto* imgui    = m_SubsystemManager.Get<DX12ImGuiSubsystem>();

	while (m_Running)
	{
		float currentTime = (float)glfwGetTime();
		float deltaTime   = currentTime - lastTime;
		lastTime          = currentTime;

		renderer->BeginFrame();
		imgui->BeginFrame();

		Renderer::BeginScene(*m_SceneCamera);
		Tick(deltaTime);
		Renderer::EndScene();

		renderer->EndFrame();
		RenderEditorLayout();
		OnImGuiRender();

		imgui->EndFrame();

		m_SubsystemManager.TickAll(deltaTime);
	}
}

void Application::RenderEditorLayout()
{
	// --- Host window ---
	ImGuiViewport* vp = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(vp->Pos);
	ImGui::SetNextWindowSize(vp->Size);
	ImGui::SetNextWindowViewport(vp->ID);

	ImGuiWindowFlags hostFlags =
		ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_MenuBar;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::Begin("##EditorHost", nullptr, hostFlags);
	ImGui::PopStyleVar(3);

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File")) { ImGui::EndMenu(); }
		if (ImGui::BeginMenu("Edit")) { ImGui::EndMenu(); }
		OnMenuBar();
		ImGui::EndMenuBar();
	}

	// --- DockSpace ---
	ImGuiID dockspaceID = ImGui::GetID("EditorDockspace");
	ImGui::DockSpace(dockspaceID, ImVec2(0, 0), ImGuiDockNodeFlags_None);

	static bool layoutBuilt = false;
	if (!layoutBuilt)
	{
		layoutBuilt = true;
		ImGui::DockBuilderRemoveNode(dockspaceID);
		ImGui::DockBuilderAddNode(dockspaceID, ImGuiDockNodeFlags_DockSpace);
		ImGui::DockBuilderSetNodeSize(dockspaceID, vp->Size);

		ImGuiID dockRight, dockCenter;
		ImGui::DockBuilderSplitNode(dockspaceID, ImGuiDir_Right, 0.25f, &dockRight, &dockCenter);

		ImGuiID dockViewport, dockBottom;
		ImGui::DockBuilderSplitNode(dockCenter, ImGuiDir_Down, 0.28f, &dockBottom, &dockViewport);

		ImGui::DockBuilderDockWindow("Viewport",        dockViewport);
		ImGui::DockBuilderDockWindow("Properties",      dockRight);
		ImGui::DockBuilderDockWindow("Content Browser", dockBottom);
		ImGui::DockBuilderFinish(dockspaceID);
	}

	ImGui::End(); // EditorHost

	// --- Viewport ---
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::Begin("Viewport");
	ImGui::PopStyleVar();

	ImVec2 viewportSize = ImGui::GetContentRegionAvail();
	auto vw = (uint32_t)viewportSize.x;
	auto vh = (uint32_t)viewportSize.y;

	if (vw > 0 && vh > 0)
	{
		static_cast<PerspectiveCamera*>(m_SceneCamera.get())->SetAspectRatio((float)vw / (float)vh);
		m_SubsystemManager.Get<DX12RendererSubsystem>()->ResizeViewport(vw, vh);
	}

	uint64_t texID = m_SubsystemManager.Get<DX12RendererSubsystem>()->GetViewportTextureID();
	ImGui::Image((ImTextureID)(uint64_t)texID, viewportSize, ImVec2(0, 0), ImVec2(1, 1));

	ImGui::End(); // Viewport

	// --- Properties ---
	ImGui::Begin("Properties");
	OnPropertiesPanel();
	ImGui::End();

	// --- Content Browser ---
	ImGui::Begin("Content Browser");
	OnContentBrowserPanel();
	ImGui::End();
}

void Application::OnEvent(Event& event)
{
	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<WindowCloseEvent>(std::bind(&Application::OnWindowClosed, this, std::placeholders::_1));

	m_SubsystemManager.OnEvent(event);

	NG_CORE_INFO("{0}", event);
}

bool Application::OnWindowClosed(WindowCloseEvent& event)
{
	m_Running = false;
	return true;
}

} // namespace Ngin
