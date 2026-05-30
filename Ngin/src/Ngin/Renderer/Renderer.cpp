#include "ngpch.h"
#include "Renderer.h"
#include "RenderCommand.h"
#include "Shader.h"

#include <cstring>

namespace Ngin {

	static float   s_ViewProjectionMatrix[16];
	static Shader* s_DefaultShader = nullptr;

	// Sun direction: slightly to the right and forward, high elevation (~60 deg)
	// Gives natural daylight shading with clear face differentiation
	static const char* s_DefaultVS = R"(
		cbuffer Constants : register(b0)
		{
			column_major float4x4 u_ViewProjection;
			column_major float4x4 u_Model;
		};
		struct VSInput { float3 Position : TEXCOORD0; float3 Normal : TEXCOORD1; };
		struct PSInput { float4 Position : SV_POSITION; float3 Normal : TEXCOORD0; };
		PSInput main(VSInput input)
		{
			PSInput output;
			output.Position = mul(u_ViewProjection, mul(u_Model, float4(input.Position, 1.0)));
			float3x3 normalMat = (float3x3)u_Model;
			output.Normal = normalize(mul(normalMat, input.Normal));
			return output;
		}
	)";

	static const char* s_DefaultPS = R"(
		struct PSInput { float4 Position : SV_POSITION; float3 Normal : TEXCOORD0; };
		float4 main(PSInput input) : SV_TARGET
		{
			float3 lightDir  = normalize(float3(0.4, 1.0, 0.3));
			float3 baseColor = float3(0.76, 0.76, 0.76);
			float  ambient   = 0.18;
			float  diffuse   = max(dot(normalize(input.Normal), lightDir), 0.0) * 0.82;
			float3 color     = baseColor * (ambient + diffuse);
			return float4(color, 1.0);
		}
	)";

	void Renderer::Init()
	{
		s_DefaultShader = Shader::Create(s_DefaultVS, s_DefaultPS);
	}

	void Renderer::Shutdown()
	{
		delete s_DefaultShader;
		s_DefaultShader = nullptr;
	}

	void Renderer::EndScene() {}

	void Renderer::BeginScene(const Camera& camera)
	{
		memcpy(s_ViewProjectionMatrix, camera.GetViewProjectionMatrix(), sizeof(float) * 16);
	}

	void Renderer::Submit(VertexArray* va, Shader* shader, const float* modelMatrix)
	{
		shader->Bind();
		shader->SetMat4("u_ViewProjection", s_ViewProjectionMatrix);
		shader->SetMat4("u_Model", modelMatrix);
		RenderCommand::DrawIndexed(va);
	}

	Shader* Renderer::GetDefaultShader()
	{
		return s_DefaultShader;
	}

}
