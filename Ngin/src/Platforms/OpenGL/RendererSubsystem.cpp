#include "ngpch.h"
#include "RendererSubsystem.h"
#include "Ngin/Log.h"
#include "Ngin/Renderer/RenderCommand.h"
#include "Ngin/Renderer/Renderer.h"
#include "Ngin/Renderer/Renderer2D.h"

#include <glad/gl.h>

namespace Ngin {

	void RendererSubsystem::OnInit()
	{
		glEnable(GL_DEPTH_TEST);
		NG_CORE_INFO("RendererSubsystem initialized. OpenGL {0}", (const char*)glGetString(GL_VERSION));
		Renderer::Init();
		Renderer2D::Init();
	}

	void RendererSubsystem::OnShutdown()
	{
		Renderer2D::Shutdown();
		Renderer::Shutdown();
		NG_CORE_INFO("RendererSubsystem shutdown.");
	}

	void RendererSubsystem::BeginFrame()
	{
		RenderCommand::SetClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		RenderCommand::Clear();
	}

}
