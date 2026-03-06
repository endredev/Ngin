#pragma once

#include "Ngin/Core.h"

namespace Ngin {

	class NG_API Camera
	{
	public:
		virtual ~Camera() = default;
		virtual const float* GetViewProjectionMatrix() const = 0;
	};

}
