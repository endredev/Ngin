#include "Application.h"

#include "Ngin/Events/ApplicationEvent.h"
#include "Ngin/Log.h"

namespace Ngin {
	Ngin::Application::Application()
	{
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

		while (true);
	}
}