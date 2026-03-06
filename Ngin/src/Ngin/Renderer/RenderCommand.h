#pragma once

#include "Ngin/Core.h"
#include "VertexArray.h"

namespace Ngin {

	class NG_API RenderCommand
	{
	public:
		static void SetClearColor(float r, float g, float b, float a);
		static void Clear();
		static void DrawIndexed(VertexArray* va, uint32_t indexCount = 0);
	};

}
