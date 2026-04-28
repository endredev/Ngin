#include "ngpch.h"

#include "Application.h"
#include "Ngin/Log.h"
#include "Platforms/Windows/WindowSubsystem.h"
#include "Platforms/Windows/InputSubsystem.h"
#include "Platforms/OpenGL/RendererSubsystem.h"
#include "Platforms/OpenGL/ImGuiSubsystem.h"

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_internal.h>

namespace Ngin {

	Application::Application()
	{
		auto* windowSys = m_SubsystemManager.Register<WindowSubsystem>();
		windowSys->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));
		m_SubsystemManager.Register<RendererSubsystem>();
		m_SubsystemManager.Register<InputSubsystem>();
		m_SubsystemManager.Register<ImGuiSubsystem>(windowSys);
		m_SubsystemManager.InitAll();
	}

	Application::~Application()
	{
		m_SubsystemManager.ShutdownAll();
	}

	void Application::Run()
	{
		OnImGuiStyle();
		float lastTime = (float)glfwGetTime();
		auto* renderer = m_SubsystemManager.Get<RendererSubsystem>();
		auto* imgui    = m_SubsystemManager.Get<ImGuiSubsystem>();

		while (m_Running)
		{
			float currentTime = (float)glfwGetTime();
			float deltaTime = currentTime - lastTime;
			lastTime = currentTime;

			renderer->BeginFrame();
			imgui->BeginFrame();

			Tick(deltaTime);

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
		auto* renderer = m_SubsystemManager.Get<RendererSubsystem>();
		renderer->ResizeViewport((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
		uint32_t texID = renderer->GetViewportTextureID();
		ImGui::Image((ImTextureID)(intptr_t)texID, viewportSize, ImVec2(0, 1), ImVec2(1, 0));

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

}
