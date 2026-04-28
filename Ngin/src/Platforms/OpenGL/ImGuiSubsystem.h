#pragma once

#include "Ngin/Core.h"
#include "Ngin/Subsystem.h"

namespace Ngin {

	class WindowSubsystem;

	struct NG_API ImGuiConfig
	{
	};

	class NG_API ImGuiSubsystem : public Subsystem
	{
	public:
		ImGuiSubsystem(WindowSubsystem* windowSys);

		void OnInit() override;
		void OnShutdown() override;

		void BeginFrame();
		void EndFrame();

	private:
		WindowSubsystem* m_WindowSys;
	};

}
