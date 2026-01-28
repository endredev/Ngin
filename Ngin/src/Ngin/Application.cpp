#include "ngpch.h"

#include "Application.h"

#include "Ngin/Events/ApplicationEvent.h"
#include "Ngin/Log.h"

#include <GLFW/glfw3.h>

namespace Ngin {
	Ngin::Application::Application()
	{
		m_Window = std::unique_ptr<Window>(Window::Create());
	}

	Ngin::Application::~Application()
	{
	}

	void Application::Run()
	{	
		WindowResizeEvent e(1280, 720);
		if (e.IsInCategory(EventCategoryApplication))
		{
			NG_TRACE(e);
		}
		if (e.IsInCategory(EventCategoryInput))
		{
			NG_TRACE(e);
		}

		while (m_Running)
		{
			glClearColor(1, 0, 1, 1);
			glClear(GL_COLOR_BUFFER_BIT);
			m_Window->OnUpdate();
		}
	}
}