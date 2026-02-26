#include "ngpch.h"

#include "WindowSubsystem.h"
#include "Ngin/Log.h"

namespace Ngin {

	void WindowSubsystem::OnInit()
	{
		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback([this](Event& e)
		{
			if (m_EventCallback)
				m_EventCallback(e);
		});

		NG_CORE_INFO("WindowSubsystem initialized.");
	}

	void WindowSubsystem::OnShutdown()
	{
		m_Window.reset();
		NG_CORE_INFO("WindowSubsystem shutdown.");
	}

	void WindowSubsystem::Tick(float deltaTime)
	{
		m_Window->OnUpdate();
	}

}
