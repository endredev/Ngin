#pragma once

#include "Ngin/Core.h"
#include "OrthographicCamera.h"

namespace Ngin {

	class NG_API Renderer2D
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginScene(const OrthographicCamera& camera);
		static void EndScene();

		static void DrawQuad(float x, float y, float width, float height,
		                     float r, float g, float b, float a = 1.0f);
	private:
		static void Flush();
	};

}
