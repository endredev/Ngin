#include "ngpch.h"
#include "ImGuiSubsystem.h"
#include "Ngin/Log.h"
#include "Platforms/Windows/WindowSubsystem.h"

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

namespace Ngin {

	ImGuiSubsystem::ImGuiSubsystem(WindowSubsystem* windowSys)
		: m_WindowSys(windowSys)
	{}

	void ImGuiSubsystem::OnInit()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		ImGui::StyleColorsDark();

		auto* glfwWindow = static_cast<GLFWwindow*>(m_WindowSys->GetWindow()->GetNativeWindow());

		float xscale, yscale;
		glfwGetWindowContentScale(glfwWindow, &xscale, &yscale);
		float scale = xscale;

		// Engine font: Segoe UI (always available on Windows)
		io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/segoeui.ttf", 16.0f * scale);

		ImGui::GetStyle().ScaleAllSizes(scale);

		ImGui_ImplGlfw_InitForOpenGL(glfwWindow, true);
		ImGui_ImplOpenGL3_Init("#version 330");

		NG_CORE_INFO("ImGuiSubsystem initialized.");
	}

	void ImGuiSubsystem::OnShutdown()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
		NG_CORE_INFO("ImGuiSubsystem shutdown.");
	}

	void ImGuiSubsystem::BeginFrame()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiSubsystem::EndFrame()
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

}
