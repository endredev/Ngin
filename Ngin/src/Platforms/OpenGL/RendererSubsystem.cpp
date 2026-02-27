#include "ngpch.h"
#include "RendererSubsystem.h"
#include "Ngin/Log.h"

#include <glad/gl.h>

namespace Ngin {

	void RendererSubsystem::OnInit()
	{
		NG_CORE_INFO("RendererSubsystem initialized. OpenGL {0}", (const char*)glGetString(GL_VERSION));
	}

	void RendererSubsystem::OnShutdown()
	{
		NG_CORE_INFO("RendererSubsystem shutdown.");
	}

	void RendererSubsystem::BeginFrame()
	{
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void RendererSubsystem::Submit(VertexArray* va, Shader* shader)
	{
		shader->Bind();
		va->Bind();
		glDrawElements(GL_TRIANGLES, va->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
	}

}
