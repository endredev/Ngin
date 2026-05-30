#include "ngpch.h"
#include "DX12Shader.h"
#include "DX12Context.h"
#include "Ngin/Log.h"

#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

namespace Ngin {

// ---- helpers ---------------------------------------------------------------

static DXGI_FORMAT ShaderDataTypeToDXGI(ShaderDataType type)
{
	switch (type)
	{
		case ShaderDataType::Float:  return DXGI_FORMAT_R32_FLOAT;
		case ShaderDataType::Float2: return DXGI_FORMAT_R32G32_FLOAT;
		case ShaderDataType::Float3: return DXGI_FORMAT_R32G32B32_FLOAT;
		case ShaderDataType::Float4: return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case ShaderDataType::Int:    return DXGI_FORMAT_R32_SINT;
		case ShaderDataType::Int2:   return DXGI_FORMAT_R32G32_SINT;
		case ShaderDataType::Int3:   return DXGI_FORMAT_R32G32B32_SINT;
		case ShaderDataType::Int4:   return DXGI_FORMAT_R32G32B32A32_SINT;
		default: break;
	}
	return DXGI_FORMAT_UNKNOWN;
}

static size_t HashLayout(const BufferLayout& layout)
{
	size_t hash = layout.GetStride();
	hash = hash * 31 + layout.GetElements().size();
	for (const auto& e : layout)
		hash = hash * 31 + (size_t)e.Type;
	return hash;
}

// ---- Mat4 identity ---------------------------------------------------------

static void SetIdentity(float* m)
{
	memset(m, 0, sizeof(float) * 16);
	m[0] = m[5] = m[10] = m[15] = 1.0f;
}

// ---- DX12Shader ------------------------------------------------------------

DX12Shader::DX12Shader(const std::string& vsSrc, const std::string& psSrc)
{
	SetIdentity(m_Constants.vp);
	SetIdentity(m_Constants.model);

	CompileShader(vsSrc, "vs_5_0", m_VSBlob);
	CompileShader(psSrc, "ps_5_0", m_PSBlob);
	CreateRootSignature();
}

void DX12Shader::CompileShader(const std::string& src, const std::string& target,
                               ComPtr<ID3DBlob>& outBlob)
{
	UINT flags = 0;
#ifdef NG_DEBUG
	flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ComPtr<ID3DBlob> errorBlob;
	HRESULT hr = D3DCompile(
		src.c_str(), src.size(), nullptr, nullptr, nullptr,
		"main", target.c_str(), flags, 0, &outBlob, &errorBlob);

	if (FAILED(hr))
	{
		if (errorBlob)
			NG_CORE_ERROR("DX12Shader compile error ({0}): {1}", target,
			              (const char*)errorBlob->GetBufferPointer());
		else
			NG_CORE_ERROR("DX12Shader compile error ({0}): unknown error", target);
	}
}

void DX12Shader::CreateRootSignature()
{
	// One root parameter: 32 root 32-bit constants at b0 (16 for VP + 16 for Model)
	D3D12_ROOT_PARAMETER rootParam = {};
	rootParam.ParameterType            = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	rootParam.Constants.ShaderRegister = 0;
	rootParam.Constants.RegisterSpace  = 0;
	rootParam.Constants.Num32BitValues = 32;
	rootParam.ShaderVisibility         = D3D12_SHADER_VISIBILITY_VERTEX;

	D3D12_ROOT_SIGNATURE_DESC rsDesc = {};
	rsDesc.NumParameters = 1;
	rsDesc.pParameters   = &rootParam;
	rsDesc.Flags         = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	ComPtr<ID3DBlob> serialized, error;
	D3D12SerializeRootSignature(&rsDesc, D3D_ROOT_SIGNATURE_VERSION_1,
	                            &serialized, &error);

	DX12Context::Get().GetDevice()->CreateRootSignature(
		0, serialized->GetBufferPointer(), serialized->GetBufferSize(),
		IID_PPV_ARGS(&m_RootSignature));
}

void DX12Shader::Bind() const
{
	DX12Context::Get().m_BoundShader = const_cast<DX12Shader*>(this);
}

void DX12Shader::SetMat4(const std::string& name, const float* matrix)
{
	if (name == "u_ViewProjection")
		memcpy(m_Constants.vp, matrix, sizeof(float) * 16);
	else if (name == "u_Model")
		memcpy(m_Constants.model, matrix, sizeof(float) * 16);
}

void DX12Shader::ApplyConstants(ID3D12GraphicsCommandList* cmdList) const
{
	cmdList->SetGraphicsRoot32BitConstants(0, 32, &m_Constants, 0);
}

ID3D12PipelineState* DX12Shader::GetOrCreatePSO(const BufferLayout& layout)
{
	size_t hash = HashLayout(layout);

	auto it = m_PSOCache.find(hash);
	if (it != m_PSOCache.end())
		return it->second.Get();

	// Build input element descs — use TEXCOORD0, TEXCOORD1, ... as semantics
	std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout;
	uint32_t slot = 0;
	for (const auto& elem : layout)
	{
		// Semantic indices: TEXCOORD0, TEXCOORD1, ...
		// We store semantic names with static storage so the pointers stay valid
		static const char* semantics[] = {
			"TEXCOORD", "TEXCOORD", "TEXCOORD", "TEXCOORD",
			"TEXCOORD", "TEXCOORD", "TEXCOORD", "TEXCOORD"
		};

		D3D12_INPUT_ELEMENT_DESC desc = {};
		desc.SemanticName         = semantics[slot];
		desc.SemanticIndex        = slot;
		desc.Format               = ShaderDataTypeToDXGI(elem.Type);
		desc.InputSlot            = 0;
		desc.AlignedByteOffset    = elem.Offset;
		desc.InputSlotClass       = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		desc.InstanceDataStepRate = 0;
		inputLayout.push_back(desc);
		slot++;
	}

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.pRootSignature        = m_RootSignature.Get();
	psoDesc.VS                    = { m_VSBlob->GetBufferPointer(), m_VSBlob->GetBufferSize() };
	psoDesc.PS                    = { m_PSBlob->GetBufferPointer(), m_PSBlob->GetBufferSize() };
	psoDesc.InputLayout           = { inputLayout.data(), (UINT)inputLayout.size() };
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets      = 1;
	psoDesc.RTVFormats[0]         = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.DSVFormat             = DXGI_FORMAT_D32_FLOAT;
	psoDesc.SampleMask            = UINT_MAX;
	psoDesc.SampleDesc.Count      = 1;

	// Rasterizer
	psoDesc.RasterizerState.FillMode              = D3D12_FILL_MODE_SOLID;
	psoDesc.RasterizerState.CullMode              = D3D12_CULL_MODE_BACK;
	psoDesc.RasterizerState.FrontCounterClockwise = TRUE;
	psoDesc.RasterizerState.DepthClipEnable       = TRUE;

	// Depth stencil
	psoDesc.DepthStencilState.DepthEnable    = TRUE;
	psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	psoDesc.DepthStencilState.DepthFunc      = D3D12_COMPARISON_FUNC_LESS;

	// Blend (default: no blending)
	psoDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	ComPtr<ID3D12PipelineState> pso;
	HRESULT hr = DX12Context::Get().GetDevice()->CreateGraphicsPipelineState(&psoDesc,
	                                                                          IID_PPV_ARGS(&pso));
	if (FAILED(hr))
	{
		NG_CORE_ERROR("DX12Shader: Failed to create PSO (hr={0:x})", (uint32_t)hr);
		return nullptr;
	}

	m_PSOCache[hash] = pso;
	return pso.Get();
}

} // namespace Ngin
