#include "ngpch.h"
#include "DX12VertexArray.h"
#include "DX12Context.h"

namespace Ngin {

void DX12VertexArray::Bind() const
{
	auto* cmdList = DX12Context::Get().GetCmdList();

	// Bind all vertex buffers and the index buffer (each calls IASetVertexBuffers/IASetIndexBuffer)
	for (const auto& vb : m_VertexBuffers)
		vb->Bind();

	if (m_IndexBuffer)
		m_IndexBuffer->Bind();

	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void DX12VertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vb)
{
	m_VertexBuffers.push_back(vb);
}

void DX12VertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& ib)
{
	m_IndexBuffer = ib;
}

} // namespace Ngin
