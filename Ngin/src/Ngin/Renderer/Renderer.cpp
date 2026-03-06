#include "ngpch.h"
#include "Renderer.h"
#include "RenderCommand.h"

#include <cstring>

namespace Ngin {

	static float s_ViewProjectionMatrix[16];

	void Renderer::Init()   {}
	void Renderer::Shutdown() {}
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

}
