#include "ngpch.h"
#include "DX12ImGuiSubsystem.h"
#include "DX12Context.h"
#include "Ngin/Log.h"
#include "Platforms/Windows/WindowSubsystem.h"

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_dx12.h>

namespace Ngin {

DX12ImGuiSubsystem::DX12ImGuiSubsystem(WindowSubsystem* windowSys)
	: m_WindowSys(windowSys)
{}

void DX12ImGuiSubsystem::OnInit()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	ImGui::StyleColorsDark();

	auto* glfwWin = static_cast<GLFWwindow*>(m_WindowSys->GetWindow()->GetNativeWindow());

	float xscale, yscale;
	glfwGetWindowContentScale(glfwWin, &xscale, &yscale);
	float scale = xscale;

	io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/segoeui.ttf", 16.0f * scale);
	io.Fonts->Build();

	ImGui::GetStyle().ScaleAllSizes(scale);

	// GLFW backend (no OpenGL — use InitForOther)
	ImGui_ImplGlfw_InitForOther(glfwWin, true);

	// DX12 backend — slot 0 of the SRV heap is reserved for the ImGui font
	auto& ctx = DX12Context::Get();
	auto  srv = ctx.AllocateSRV();

	ImGui_ImplDX12_Init(
		ctx.GetDevice(),
		DX12_FRAMES_IN_FLIGHT,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		ctx.GetSrvHeap(),
		srv.cpu,
		srv.gpu);

	NG_CORE_INFO("DX12ImGuiSubsystem initialized.");
}

void DX12ImGuiSubsystem::OnShutdown()
{
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	NG_CORE_INFO("DX12ImGuiSubsystem shutdown.");
}

void DX12ImGuiSubsystem::BeginFrame()
{
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void DX12ImGuiSubsystem::EndFrame()
{
	ImGui::Render();

	auto& ctx     = DX12Context::Get();
	auto* cmdList = ctx.GetCmdList();
	auto* bb      = ctx.GetBackBuffer();
	auto  bbRTV   = ctx.GetBackBufferRTV();

	// Transition back buffer: PRESENT → RENDER_TARGET
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource   = bb;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	cmdList->ResourceBarrier(1, &barrier);

	// Bind back buffer as render target (no depth — ImGui is 2D)
	cmdList->OMSetRenderTargets(1, &bbRTV, FALSE, nullptr);

	// Clear back buffer to black (ImGui draws on top)
	const float black[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	cmdList->ClearRenderTargetView(bbRTV, black, 0, nullptr);

	// Set the shader-visible SRV heap so ImGui shaders can sample textures
	ID3D12DescriptorHeap* heaps[] = { ctx.GetSrvHeap() };
	cmdList->SetDescriptorHeaps(1, heaps);

	// Record ImGui draw calls
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), cmdList);

	// Transition back buffer: RENDER_TARGET → PRESENT
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_PRESENT;
	cmdList->ResourceBarrier(1, &barrier);

	// Execute command list and present
	ctx.ExecuteAndPresent();
}

} // namespace Ngin
