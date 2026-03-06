#include "ngpch.h"

#include "Application.h"
#include "Ngin/Log.h"
#include "Platforms/Windows/WindowSubsystem.h"
#include "Platforms/Windows/InputSubsystem.h"
#include "Platforms/OpenGL/RendererSubsystem.h"

#include <GLFW/glfw3.h>

namespace Ngin {

	Application::Application()
	{
		auto* windowSys = m_SubsystemManager.Register<WindowSubsystem>();
		windowSys->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));
		m_SubsystemManager.Register<RendererSubsystem>();
		m_SubsystemManager.Register<InputSubsystem>();
		m_SubsystemManager.InitAll();
	}

	Application::~Application()
	{
		m_SubsystemManager.ShutdownAll();
	}

	void Application::Run()
	{
		float lastTime = (float)glfwGetTime();
		auto* renderer = m_SubsystemManager.Get<RendererSubsystem>();

		while (m_Running)
		{
			float currentTime = (float)glfwGetTime();
			float deltaTime = currentTime - lastTime;
			lastTime = currentTime;

			renderer->BeginFrame();

			// App tick
			Tick(deltaTime);

			// All the sub systems tick
			m_SubsystemManager.TickAll(deltaTime);
		}
	}

	void Application::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<WindowCloseEvent>(std::bind(&Application::OnWindowClosed, this, std::placeholders::_1));

		m_SubsystemManager.OnEvent(event);

		NG_CORE_INFO("{0}", event);
	}

	bool Application::OnWindowClosed(WindowCloseEvent& event)
	{
		m_Running = false;
		return true;
	}

}
