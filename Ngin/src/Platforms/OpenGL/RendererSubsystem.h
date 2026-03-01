#pragma once

#include "Ngin/Subsystem.h"

namespace Ngin {

	class NG_API RendererSubsystem : public Subsystem
	{
	public:
		void OnInit() override;
		void OnShutdown() override;

		void BeginFrame();
	};

}
