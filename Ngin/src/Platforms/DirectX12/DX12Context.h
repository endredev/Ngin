#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>

#include <cstdint>

using Microsoft::WRL::ComPtr;

namespace Ngin {

static constexpr uint32_t DX12_FRAMES_IN_FLIGHT = 2;

class DX12Shader;

class DX12Context
{
public:
	static void Init(HWND hwnd, uint32_t width, uint32_t height);
	static void Shutdown();
	static DX12Context& Get();

	void BeginFrame();
	void ExecuteAndPresent();
	void WaitIdle();

	// Accessors
	ID3D12Device*              GetDevice()    const { return m_Device.Get(); }
	ID3D12GraphicsCommandList* GetCmdList()   const { return m_CmdList.Get(); }
	ID3D12DescriptorHeap*      GetSrvHeap()   const { return m_SrvHeap.Get(); }
	uint32_t                   GetFrameIndex() const { return m_FrameIndex; }
	uint32_t                   GetWidth()     const { return m_Width; }
	uint32_t                   GetHeight()    const { return m_Height; }

	ID3D12Resource*                GetBackBuffer() const { return m_BackBuffers[m_FrameIndex].Get(); }
	D3D12_CPU_DESCRIPTOR_HANDLE    GetBackBufferRTV() const { return m_BackBufferRtvs[m_FrameIndex]; }

	// Descriptor allocation
	D3D12_CPU_DESCRIPTOR_HANDLE AllocateRTV();

	struct SrvAllocation
	{
		D3D12_CPU_DESCRIPTOR_HANDLE cpu;
		D3D12_GPU_DESCRIPTOR_HANDLE gpu;
	};
	SrvAllocation AllocateSRV();
	D3D12_CPU_DESCRIPTOR_HANDLE AllocateDSV();

	UINT GetRtvDescSize() const { return m_RtvDescSize; }
	UINT GetSrvDescSize() const { return m_SrvDescSize; }
	UINT GetDsvDescSize() const { return m_DsvDescSize; }

	// State used by DX12RenderCommand::Clear
	D3D12_CPU_DESCRIPTOR_HANDLE m_CurrentRTV = {};
	D3D12_CPU_DESCRIPTOR_HANDLE m_CurrentDSV = {};
	bool                        m_HasCurrentDSV = false;
	float                       m_ClearColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f };

	// Currently bound shader (set by DX12Shader::Bind, used by DrawIndexed)
	DX12Shader* m_BoundShader = nullptr;

private:
	void InitInternal(HWND hwnd, uint32_t width, uint32_t height);
	void WaitForFrame(uint32_t frameIndex);

	ComPtr<ID3D12Device>              m_Device;
	ComPtr<ID3D12CommandQueue>        m_CommandQueue;
	ComPtr<IDXGISwapChain3>           m_SwapChain;

	ComPtr<ID3D12CommandAllocator>    m_CmdAllocators[DX12_FRAMES_IN_FLIGHT];
	ComPtr<ID3D12GraphicsCommandList> m_CmdList;

	ComPtr<ID3D12Fence>               m_Fence;
	HANDLE                            m_FenceEvent = nullptr;
	uint64_t                          m_FenceValues[DX12_FRAMES_IN_FLIGHT] = {};
	uint64_t                          m_FenceCounter = 0;

	ComPtr<ID3D12Resource>            m_BackBuffers[DX12_FRAMES_IN_FLIGHT];
	D3D12_CPU_DESCRIPTOR_HANDLE       m_BackBufferRtvs[DX12_FRAMES_IN_FLIGHT] = {};

	ComPtr<ID3D12DescriptorHeap>      m_RtvHeap;
	ComPtr<ID3D12DescriptorHeap>      m_SrvHeap;
	ComPtr<ID3D12DescriptorHeap>      m_DsvHeap;

	UINT m_RtvDescSize = 0;
	UINT m_SrvDescSize = 0;
	UINT m_DsvDescSize = 0;

	UINT m_NextRtvSlot = DX12_FRAMES_IN_FLIGHT;
	UINT m_NextSrvSlot = 0;
	UINT m_NextDsvSlot = 0;

	uint32_t m_FrameIndex = 0;
	uint32_t m_Width  = 0;
	uint32_t m_Height = 0;

	static DX12Context* s_Instance;
};

} // namespace Ngin
