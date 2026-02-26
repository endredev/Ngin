#include "ngpch.h"

#include "Application.h"
#include "Ngin/Log.h"
#include "Platforms/WindowSubsystem.h"

#include <GLFW/glfw3.h>

namespace Ngin {

	Application::Application()
	{
		auto* windowSys = m_SubsystemManager.Register<WindowSubsystem>();
		windowSys->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));
		m_SubsystemManager.InitAll();
	}

	Application::~Application()
	{
		m_SubsystemManager.ShutdownAll();
	}

	void Application::Run()
	{
		float lastTime = (float)glfwGetTime();

		while (m_Running)
		{
			float currentTime = (float)glfwGetTime();
			float deltaTime = currentTime - lastTime;
			lastTime = currentTime;

			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

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
