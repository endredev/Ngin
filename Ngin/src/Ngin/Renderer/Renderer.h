#pragma once

#include "Ngin/Core.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Camera.h"

namespace Ngin {

	class NG_API Renderer
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginScene(const Camera& camera);
		static void EndScene();

		static void Submit(VertexArray* va, Shader* shader, const float* modelMatrix);
	};

}
