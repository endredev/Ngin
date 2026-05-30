#include "ngpch.h"
#include "DX12Buffer.h"
#include "DX12Context.h"

namespace Ngin {

// ---- Helpers ---------------------------------------------------------------

static ComPtr<ID3D12Resource> CreateUploadBuffer(ID3D12Device* device,
                                                  const void* data, uint32_t size)
{
	D3D12_HEAP_PROPERTIES heapProps = {};
	heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension          = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Width              = size;
	desc.Height             = 1;
	desc.DepthOrArraySize   = 1;
	desc.MipLevels          = 1;
	desc.Format             = DXGI_FORMAT_UNKNOWN;
	desc.SampleDesc.Count   = 1;
	desc.Layout             = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	desc.Flags              = D3D12_RESOURCE_FLAG_NONE;

	ComPtr<ID3D12Resource> buffer;
	device->CreateCommittedResource(
		&heapProps, D3D12_HEAP_FLAG_NONE,
		&desc, D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr, IID_PPV_ARGS(&buffer));

	if (data)
	{
		void* mapped = nullptr;
		buffer->Map(0, nullptr, &mapped);
		memcpy(mapped, data, size);
		buffer->Unmap(0, nullptr);
	}

	return buffer;
}

// ---- DX12VertexBuffer ------------------------------------------------------

DX12VertexBuffer::DX12VertexBuffer(float* vertices, uint32_t size)
	: m_Size(size)
{
	Allocate(vertices, size);
}

DX12VertexBuffer::DX12VertexBuffer(uint32_t size)
	: m_Size(size)
{
	Allocate(nullptr, size);
}

void DX12VertexBuffer::Allocate(const void* data, uint32_t size)
{
	m_Buffer = CreateUploadBuffer(DX12Context::Get().GetDevice(), data, size);
	// View will be updated when SetLayout is called
	m_View.BufferLocation = m_Buffer->GetGPUVirtualAddress();
	m_View.SizeInBytes    = size;
	m_View.StrideInBytes  = 0; // set in UpdateView() once layout is known
}

void DX12VertexBuffer::UpdateView()
{
	m_View.StrideInBytes = m_Layout.GetStride();
}

void DX12VertexBuffer::Bind() const
{
	DX12Context::Get().GetCmdList()->IASetVertexBuffers(0, 1, &m_View);
}

void DX12VertexBuffer::SetData(const void* data, uint32_t size)
{
	void* mapped = nullptr;
	m_Buffer->Map(0, nullptr, &mapped);
	memcpy(mapped, data, size);
	m_Buffer->Unmap(0, nullptr);
}

// ---- DX12IndexBuffer -------------------------------------------------------

DX12IndexBuffer::DX12IndexBuffer(uint32_t* indices, uint32_t count)
	: m_Count(count)
{
	uint32_t size = count * sizeof(uint32_t);
	m_Buffer = CreateUploadBuffer(DX12Context::Get().GetDevice(), indices, size);

	m_View.BufferLocation = m_Buffer->GetGPUVirtualAddress();
	m_View.SizeInBytes    = size;
	m_View.Format         = DXGI_FORMAT_R32_UINT;
}

void DX12IndexBuffer::Bind() const
{
	DX12Context::Get().GetCmdList()->IASetIndexBuffer(&m_View);
}

} // namespace Ngin
