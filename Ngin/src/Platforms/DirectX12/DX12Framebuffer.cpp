#include "ngpch.h"
#include "DX12Framebuffer.h"
#include "DX12Context.h"
#include "Ngin/Log.h"

namespace Ngin {

DX12Framebuffer::DX12Framebuffer(uint32_t width, uint32_t height)
	: m_Width(width), m_Height(height)
{
	Invalidate();
}

DX12Framebuffer::~DX12Framebuffer()
{
	Release();
}

void DX12Framebuffer::Release()
{
	m_RenderTarget.Reset();
	m_DepthStencil.Reset();
}

void DX12Framebuffer::Invalidate()
{
	Release();

	auto* device = DX12Context::Get().GetDevice();

	// --- Render target texture ---
	{
		D3D12_HEAP_PROPERTIES heapProps = {};
		heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

		D3D12_RESOURCE_DESC desc = {};
		desc.Dimension          = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Width              = m_Width;
		desc.Height             = m_Height;
		desc.DepthOrArraySize   = 1;
		desc.MipLevels          = 1;
		desc.Format             = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count   = 1;
		desc.Flags              = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

		D3D12_CLEAR_VALUE clearVal = {};
		clearVal.Format   = DXGI_FORMAT_R8G8B8A8_UNORM;
		clearVal.Color[0] = 0.1f; clearVal.Color[1] = 0.1f;
		clearVal.Color[2] = 0.1f; clearVal.Color[3] = 1.0f;

		device->CreateCommittedResource(
			&heapProps, D3D12_HEAP_FLAG_NONE,
			&desc, D3D12_RESOURCE_STATE_RENDER_TARGET,
			&clearVal, IID_PPV_ARGS(&m_RenderTarget));

		m_RTState = D3D12_RESOURCE_STATE_RENDER_TARGET;
	}

	// --- Depth stencil ---
	{
		D3D12_HEAP_PROPERTIES heapProps = {};
		heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

		D3D12_RESOURCE_DESC desc = {};
		desc.Dimension          = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Width              = m_Width;
		desc.Height             = m_Height;
		desc.DepthOrArraySize   = 1;
		desc.MipLevels          = 1;
		desc.Format             = DXGI_FORMAT_D32_FLOAT;
		desc.SampleDesc.Count   = 1;
		desc.Flags              = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		D3D12_CLEAR_VALUE clearVal = {};
		clearVal.Format               = DXGI_FORMAT_D32_FLOAT;
		clearVal.DepthStencil.Depth   = 1.0f;
		clearVal.DepthStencil.Stencil = 0;

		device->CreateCommittedResource(
			&heapProps, D3D12_HEAP_FLAG_NONE,
			&desc, D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&clearVal, IID_PPV_ARGS(&m_DepthStencil));

		m_DSState = D3D12_RESOURCE_STATE_DEPTH_WRITE;
	}

	// --- Descriptors (allocate once, reuse on resize) ---
	auto& ctx = DX12Context::Get();

	if (!m_Allocated)
	{
		m_RTV    = ctx.AllocateRTV();
		m_DSV    = ctx.AllocateDSV();
		auto srv = ctx.AllocateSRV();
		m_SrvCpu = srv.cpu;
		m_SrvGpu = srv.gpu;
		m_Allocated = true;
	}

	// RTV
	device->CreateRenderTargetView(m_RenderTarget.Get(), nullptr, m_RTV);

	// DSV
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format        = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	device->CreateDepthStencilView(m_DepthStencil.Get(), &dsvDesc, m_DSV);

	// SRV (so ImGui can sample this texture)
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format                  = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension           = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Texture2D.MipLevels     = 1;
	device->CreateShaderResourceView(m_RenderTarget.Get(), &srvDesc, m_SrvCpu);
}

void DX12Framebuffer::Bind() const
{
	auto* cmdList = DX12Context::Get().GetCmdList();

	// Transition RT to RENDER_TARGET if needed
	if (m_RTState != D3D12_RESOURCE_STATE_RENDER_TARGET)
	{
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Transition.pResource   = m_RenderTarget.Get();
		barrier.Transition.StateBefore = m_RTState;
		barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		cmdList->ResourceBarrier(1, &barrier);
		m_RTState = D3D12_RESOURCE_STATE_RENDER_TARGET;
	}

	// Set viewport and scissor
	D3D12_VIEWPORT vp = { 0.0f, 0.0f, (float)m_Width, (float)m_Height, 0.0f, 1.0f };
	D3D12_RECT     sc = { 0, 0, (LONG)m_Width, (LONG)m_Height };
	cmdList->RSSetViewports(1, &vp);
	cmdList->RSSetScissorRects(1, &sc);

	// Bind as render target
	cmdList->OMSetRenderTargets(1, &m_RTV, FALSE, &m_DSV);

	// Store handles for DX12RenderCommand::Clear
	auto& ctx = DX12Context::Get();
	ctx.m_CurrentRTV    = m_RTV;
	ctx.m_CurrentDSV    = m_DSV;
	ctx.m_HasCurrentDSV = true;
}

void DX12Framebuffer::Unbind() const
{
	// Transition RT to PIXEL_SHADER_RESOURCE so ImGui can sample it
	if (m_RTState != D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE)
	{
		auto* cmdList = DX12Context::Get().GetCmdList();

		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Transition.pResource   = m_RenderTarget.Get();
		barrier.Transition.StateBefore = m_RTState;
		barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		cmdList->ResourceBarrier(1, &barrier);
		m_RTState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	}
}

void DX12Framebuffer::Resize(uint32_t width, uint32_t height)
{
	if (width == 0 || height == 0 || (width == m_Width && height == m_Height))
		return;

	DX12Context::Get().WaitIdle();

	m_Width  = width;
	m_Height = height;
	Invalidate();
}

} // namespace Ngin
