#pragma once
#include "Core.h"
#include "Events/Event.h"
#include "SubsystemManager.h"
#include "Ngin/Events/ApplicationEvent.h"
#include "Ngin/Renderer/Camera.h"

#include <memory>

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
		virtual void OnMenuBar() {}
		virtual void OnPropertiesPanel() {}
		virtual void OnContentBrowserPanel() {}

		SubsystemManager& GetSubsystemManager() { return m_SubsystemManager; }

	protected:
		Camera& GetSceneCamera() { return *m_SceneCamera; }

	private:
		bool OnWindowClosed(WindowCloseEvent& event);
		void RenderEditorLayout();

		SubsystemManager         m_SubsystemManager;
		std::unique_ptr<Camera>  m_SceneCamera;
		bool                     m_Running = true;
	};

	// To be defined in client
	Application* CreateApplication();
}
