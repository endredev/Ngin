#include "ngpch.h"
#include "RendererSubsystem.h"
#include "Ngin/Log.h"
#include "Ngin/Renderer/RenderCommand.h"

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
		RenderCommand::SetClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		RenderCommand::Clear();
	}

}
