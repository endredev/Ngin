#pragma once
#include "Core.h"
#include "Events/Event.h"
#include "Window.h"
#include "Ngin/Events/ApplicationEvent.h"

namespace Ngin {
	class NG_API Application
	{
	public: 
		Application();
		virtual ~Application();
		void Run();

		void OnEvent(Event& event);
	private:
		bool OnWindowClosed(WindowCloseEvent& event);

		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
	};

	// To be defined in client
	Application* CreateApplication();
}

