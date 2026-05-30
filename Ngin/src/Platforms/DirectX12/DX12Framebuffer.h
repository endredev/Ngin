#pragma once

#include "Ngin/Renderer/Framebuffer.h"
#include <d3d12.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

namespace Ngin {

class DX12Framebuffer : public Framebuffer
{
public:
	DX12Framebuffer(uint32_t width, uint32_t height);
	~DX12Framebuffer();

	void Bind()   const override;  // transition to RT, set as render target
	void Unbind() const override;  // transition to SRV

	void Resize(uint32_t width, uint32_t height) override;

	// Returns the GPU descriptor handle of the SRV (for ImGui::Image)
	uint64_t GetColorAttachmentID() const override { return m_SrvGpu.ptr; }
	uint32_t GetWidth()  const override { return m_Width; }
	uint32_t GetHeight() const override { return m_Height; }

private:
	void Invalidate();
	void Release();

	ComPtr<ID3D12Resource> m_RenderTarget;
	ComPtr<ID3D12Resource> m_DepthStencil;

	D3D12_CPU_DESCRIPTOR_HANDLE m_RTV    = {};
	D3D12_CPU_DESCRIPTOR_HANDLE m_DSV    = {};
	D3D12_CPU_DESCRIPTOR_HANDLE m_SrvCpu = {};
	D3D12_GPU_DESCRIPTOR_HANDLE m_SrvGpu = {};

	mutable D3D12_RESOURCE_STATES m_RTState  = D3D12_RESOURCE_STATE_RENDER_TARGET;
	mutable D3D12_RESOURCE_STATES m_DSState  = D3D12_RESOURCE_STATE_DEPTH_WRITE;

	uint32_t m_Width  = 0;
	uint32_t m_Height = 0;

	// Track if descriptors have been allocated (so we reuse them on Resize)
	bool m_Allocated = false;
};

} // namespace Ngin
