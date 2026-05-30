#include "ngpch.h"

#include "Ngin/Renderer/RenderCommand.h"
#include "Ngin/Renderer/VertexArray.h"
#include "DX12Context.h"
#include "DX12Shader.h"

namespace Ngin {

void RenderCommand::SetClearColor(float r, float g, float b, float a)
{
	auto& ctx = DX12Context::Get();
	ctx.m_ClearColor[0] = r;
	ctx.m_ClearColor[1] = g;
	ctx.m_ClearColor[2] = b;
	ctx.m_ClearColor[3] = a;
}

void RenderCommand::Clear()
{
	auto& ctx     = DX12Context::Get();
	auto* cmdList = ctx.GetCmdList();

	cmdList->ClearRenderTargetView(ctx.m_CurrentRTV, ctx.m_ClearColor, 0, nullptr);

	if (ctx.m_HasCurrentDSV)
		cmdList->ClearDepthStencilView(ctx.m_CurrentDSV,
		                               D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void RenderCommand::DrawIndexed(VertexArray* va, uint32_t indexCount)
{
	auto& ctx     = DX12Context::Get();
	auto* cmdList = ctx.GetCmdList();
	auto* shader  = ctx.m_BoundShader;

	if (!shader) return;

	// Retrieve the layout from the first vertex buffer
	const BufferLayout& layout = va->GetVertexBuffers()[0]->GetLayout();

	auto* pso = shader->GetOrCreatePSO(layout);
	if (!pso) return;

	cmdList->SetGraphicsRootSignature(shader->GetRootSignature());
	cmdList->SetPipelineState(pso);
	shader->ApplyConstants(cmdList);

	// Bind buffers and topology
	va->Bind();

	uint32_t count = indexCount ? indexCount : va->GetIndexBuffer()->GetCount();
	cmdList->DrawIndexedInstanced(count, 1, 0, 0, 0);
}

} // namespace Ngin
