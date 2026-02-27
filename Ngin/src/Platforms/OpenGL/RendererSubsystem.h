#pragma once

#include "Ngin/Subsystem.h"
#include "Ngin/Renderer/VertexArray.h"
#include "Ngin/Renderer/Shader.h"

namespace Ngin {

	class NG_API RendererSubsystem : public Subsystem
	{
	public:
		void OnInit() override;
		void OnShutdown() override;

		void BeginFrame();
		void Submit(VertexArray* va, Shader* shader);
	};

}
