#pragma once

#include "Ngin/Renderer/Buffer.h"
#include <d3d12.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

namespace Ngin {

class DX12VertexBuffer : public VertexBuffer
{
public:
	DX12VertexBuffer(float* vertices, uint32_t size);  // static data
	DX12VertexBuffer(uint32_t size);                   // dynamic data
	virtual ~DX12VertexBuffer() = default;

	void Bind()   const override;
	void Unbind() const override {}

	void SetLayout(const BufferLayout& layout) override { m_Layout = layout; UpdateView(); }
	const BufferLayout& GetLayout() const override { return m_Layout; }

	void SetData(const void* data, uint32_t size) override;

private:
	void Allocate(const void* data, uint32_t size);
	void UpdateView();

	ComPtr<ID3D12Resource>   m_Buffer;
	D3D12_VERTEX_BUFFER_VIEW m_View = {};
	BufferLayout             m_Layout;
	uint32_t                 m_Size = 0;
};

class DX12IndexBuffer : public IndexBuffer
{
public:
	DX12IndexBuffer(uint32_t* indices, uint32_t count);
	virtual ~DX12IndexBuffer() = default;

	void Bind()   const override;
	void Unbind() const override {}

	uint32_t GetCount() const override { return m_Count; }

private:
	ComPtr<ID3D12Resource>  m_Buffer;
	D3D12_INDEX_BUFFER_VIEW m_View = {};
	uint32_t                m_Count = 0;
};

} // namespace Ngin
