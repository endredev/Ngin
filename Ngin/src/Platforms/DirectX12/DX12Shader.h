#pragma once

#include "Ngin/Renderer/Shader.h"
#include "Ngin/Renderer/Buffer.h"
#include <d3d12.h>
#include <wrl/client.h>
#include <unordered_map>

using Microsoft::WRL::ComPtr;

namespace Ngin {

class DX12Shader : public Shader
{
public:
	DX12Shader(const std::string& vsSrc, const std::string& psSrc);
	virtual ~DX12Shader() = default;

	void Bind()   const override;
	void Unbind() const override {}

	// Maps "u_ViewProjection" → first 16 constants, "u_Model" → next 16
	void SetMat4(const std::string& name, const float* matrix) override;

	// Called by DX12RenderCommand::DrawIndexed to get the right PSO for the current layout
	ID3D12PipelineState* GetOrCreatePSO(const BufferLayout& layout);
	ID3D12RootSignature* GetRootSignature() const { return m_RootSignature.Get(); }
	void ApplyConstants(ID3D12GraphicsCommandList* cmdList) const;

private:
	void CompileShader(const std::string& src, const std::string& target,
	                   ComPtr<ID3DBlob>& outBlob);
	void CreateRootSignature();

	ComPtr<ID3DBlob>         m_VSBlob;
	ComPtr<ID3DBlob>         m_PSBlob;
	ComPtr<ID3D12RootSignature> m_RootSignature;

	// PSO cache keyed by layout hash
	mutable std::unordered_map<size_t, ComPtr<ID3D12PipelineState>> m_PSOCache;

	// Root constants: [0..15] = ViewProjection, [16..31] = Model
	struct ShaderConstants
	{
		float vp[16];
		float model[16];
	};
	mutable ShaderConstants m_Constants;
};

} // namespace Ngin
