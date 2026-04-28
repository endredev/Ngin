#pragma once
#include "Core.h"
#include "Events/Event.h"
#include "SubsystemManager.h"
#include "Ngin/Events/ApplicationEvent.h"

namespace Ngin {
	class NG_API Application
	{
	public:
		Application();
		virtual ~Application();
		void Run();

		void OnEvent(Event& event);
		virtual void Tick(float deltaTime) {}
		virtual void OnImGuiRender() {}
		virtual void OnImGuiStyle() {}
		virtual void OnMenuBar() {}
		virtual void OnPropertiesPanel() {}
		virtual void OnContentBrowserPanel() {}

		SubsystemManager& GetSubsystemManager() { return m_SubsystemManager; }
	private:
		bool OnWindowClosed(WindowCloseEvent& event);
		void RenderEditorLayout();

		SubsystemManager m_SubsystemManager;
		bool m_Running = true;
	};

	// To be defined in client
	Application* CreateApplication();
}

