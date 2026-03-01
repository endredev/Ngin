#include "ngpch.h"
#include "Ngin/Renderer/RenderCommand.h"

#include <glad/gl.h>

namespace Ngin {

	void RenderCommand::SetClearColor(float r, float g, float b, float a)
	{
		glClearColor(r, g, b, a);
	}

	void RenderCommand::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void RenderCommand::DrawIndexed(VertexArray* va, uint32_t indexCount)
	{
		va->Bind();
		uint32_t count = indexCount ? indexCount : va->GetIndexBuffer()->GetCount();
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
	}

}
