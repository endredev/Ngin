#include "ngpch.h"
#include "DX12Context.h"
#include "Ngin/Log.h"

#include <d3d12.h>
#include <dxgi1_6.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

namespace Ngin {

DX12Context* DX12Context::s_Instance = nullptr;

void DX12Context::Init(HWND hwnd, uint32_t width, uint32_t height)
{
	s_Instance = new DX12Context();
	s_Instance->InitInternal(hwnd, width, height);
}

void DX12Context::Shutdown()
{
	if (!s_Instance) return;

	// Wait for all GPU work to finish before releasing resources
	for (uint32_t i = 0; i < DX12_FRAMES_IN_FLIGHT; i++)
		s_Instance->WaitForFrame(i);

	CloseHandle(s_Instance->m_FenceEvent);
	delete s_Instance;
	s_Instance = nullptr;
}

DX12Context& DX12Context::Get()
{
	return *s_Instance;
}

void DX12Context::InitInternal(HWND hwnd, uint32_t width, uint32_t height)
{
	m_Width  = width;
	m_Height = height;

#ifdef NG_DEBUG
	ComPtr<ID3D12Debug> debugCtrl;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugCtrl))))
		debugCtrl->EnableDebugLayer();
#endif

	ComPtr<IDXGIFactory4> factory;
	CreateDXGIFactory1(IID_PPV_ARGS(&factory));

	// Find hardware adapter
	ComPtr<IDXGIAdapter1> adapter;
	for (UINT i = 0; factory->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND; i++)
	{
		DXGI_ADAPTER_DESC1 desc;
		adapter->GetDesc1(&desc);
		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;
		if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0,
		                                _uuidof(ID3D12Device), nullptr)))
			break;
	}

	HRESULT hr = D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0,
	                               IID_PPV_ARGS(&m_Device));
	if (FAILED(hr))
	{
		NG_CORE_ERROR("DX12Context: Failed to create D3D12 device (hr={0:x})", (uint32_t)hr);
		return;
	}

	// Command queue
	D3D12_COMMAND_QUEUE_DESC qDesc = {};
	qDesc.Type  = D3D12_COMMAND_LIST_TYPE_DIRECT;
	qDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	m_Device->CreateCommandQueue(&qDesc, IID_PPV_ARGS(&m_CommandQueue));

	// Swap chain
	DXGI_SWAP_CHAIN_DESC1 scDesc = {};
	scDesc.BufferCount  = DX12_FRAMES_IN_FLIGHT;
	scDesc.Width        = width;
	scDesc.Height       = height;
	scDesc.Format       = DXGI_FORMAT_R8G8B8A8_UNORM;
	scDesc.BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scDesc.SwapEffect   = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	scDesc.SampleDesc.Count = 1;

	ComPtr<IDXGISwapChain1> sc1;
	factory->CreateSwapChainForHwnd(m_CommandQueue.Get(), hwnd, &scDesc,
	                                nullptr, nullptr, &sc1);
	factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);
	sc1.As(&m_SwapChain);
	m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();

	// Descriptor heaps
	{
		D3D12_DESCRIPTOR_HEAP_DESC d = {};
		d.NumDescriptors = 128;
		d.Type  = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		m_Device->CreateDescriptorHeap(&d, IID_PPV_ARGS(&m_RtvHeap));
		m_RtvDescSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}
	{
		D3D12_DESCRIPTOR_HEAP_DESC d = {};
		d.NumDescriptors = 128;
		d.Type  = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		d.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		m_Device->CreateDescriptorHeap(&d, IID_PPV_ARGS(&m_SrvHeap));
		m_SrvDescSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}
	{
		D3D12_DESCRIPTOR_HEAP_DESC d = {};
		d.NumDescriptors = 64;
		d.Type  = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		m_Device->CreateDescriptorHeap(&d, IID_PPV_ARGS(&m_DsvHeap));
		m_DsvDescSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	}

	// RTVs for back buffers
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_RtvHeap->GetCPUDescriptorHandleForHeapStart();
	for (uint32_t i = 0; i < DX12_FRAMES_IN_FLIGHT; i++)
	{
		m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&m_BackBuffers[i]));
		m_Device->CreateRenderTargetView(m_BackBuffers[i].Get(), nullptr, rtvHandle);
		m_BackBufferRtvs[i] = rtvHandle;
		rtvHandle.ptr += m_RtvDescSize;
	}
	m_NextRtvSlot = DX12_FRAMES_IN_FLIGHT;

	// Command allocators
	for (uint32_t i = 0; i < DX12_FRAMES_IN_FLIGHT; i++)
		m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
		                                 IID_PPV_ARGS(&m_CmdAllocators[i]));

	// Command list (created in recording state, close immediately)
	m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
	                            m_CmdAllocators[m_FrameIndex].Get(), nullptr,
	                            IID_PPV_ARGS(&m_CmdList));
	m_CmdList->Close();

	// Fence
	m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence));
	m_FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

	NG_CORE_INFO("DX12Context initialized.");
}

void DX12Context::BeginFrame()
{
	// Wait until this frame slot is free
	WaitForFrame(m_FrameIndex);

	m_CmdAllocators[m_FrameIndex]->Reset();
	m_CmdList->Reset(m_CmdAllocators[m_FrameIndex].Get(), nullptr);
}

void DX12Context::ExecuteAndPresent()
{
	m_CmdList->Close();

	ID3D12CommandList* lists[] = { m_CmdList.Get() };
	m_CommandQueue->ExecuteCommandLists(1, lists);

	m_SwapChain->Present(1, 0);

	m_FenceCounter++;
	m_FenceValues[m_FrameIndex] = m_FenceCounter;
	m_CommandQueue->Signal(m_Fence.Get(), m_FenceCounter);

	m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();
}

void DX12Context::WaitForFrame(uint32_t frameIndex)
{
	if (m_Fence->GetCompletedValue() < m_FenceValues[frameIndex])
	{
		m_Fence->SetEventOnCompletion(m_FenceValues[frameIndex], m_FenceEvent);
		WaitForSingleObject(m_FenceEvent, INFINITE);
	}
}

void DX12Context::WaitIdle()
{
	for (uint32_t i = 0; i < DX12_FRAMES_IN_FLIGHT; i++)
		WaitForFrame(i);
}

D3D12_CPU_DESCRIPTOR_HANDLE DX12Context::AllocateRTV()
{
	D3D12_CPU_DESCRIPTOR_HANDLE handle = m_RtvHeap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += (SIZE_T)m_NextRtvSlot * m_RtvDescSize;
	m_NextRtvSlot++;
	return handle;
}

DX12Context::SrvAllocation DX12Context::AllocateSRV()
{
	SrvAllocation alloc;
	alloc.cpu = m_SrvHeap->GetCPUDescriptorHandleForHeapStart();
	alloc.cpu.ptr += (SIZE_T)m_NextSrvSlot * m_SrvDescSize;
	alloc.gpu = m_SrvHeap->GetGPUDescriptorHandleForHeapStart();
	alloc.gpu.ptr += (UINT64)m_NextSrvSlot * m_SrvDescSize;
	m_NextSrvSlot++;
	return alloc;
}

D3D12_CPU_DESCRIPTOR_HANDLE DX12Context::AllocateDSV()
{
	D3D12_CPU_DESCRIPTOR_HANDLE handle = m_DsvHeap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += (SIZE_T)m_NextDsvSlot * m_DsvDescSize;
	m_NextDsvSlot++;
	return handle;
}

} // namespace Ngin
