#pragma once

#include "Ngin/Core.h"
#include "Ngin/Subsystem.h"

namespace Ngin {

class WindowSubsystem;

class NG_API DX12ImGuiSubsystem : public Subsystem
{
public:
	explicit DX12ImGuiSubsystem(WindowSubsystem* windowSys);

	void OnInit()     override;
	void OnShutdown() override;

	void BeginFrame();
	void EndFrame();    // renders ImGui to back buffer, executes cmd list, presents

private:
	WindowSubsystem* m_WindowSys;
};

} // namespace Ngin
