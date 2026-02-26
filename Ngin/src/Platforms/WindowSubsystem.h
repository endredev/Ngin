#pragma once

#include "Ngin/Subsystem.h"
#include "Ngin/Window.h"

#include <functional>
#include <memory>

namespace Ngin {

	class NG_API WindowSubsystem : public Subsystem
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		void OnInit() override;
		void OnShutdown() override;
		void Tick(float deltaTime) override;

		void SetEventCallback(const EventCallbackFn& callback) { m_EventCallback = callback; }

		Window* GetWindow() { return m_Window.get(); }
		unsigned int GetWidth() const { return m_Window->GetWidth(); }
		unsigned int GetHeight() const { return m_Window->GetHeight(); }

	private:
		std::unique_ptr<Window> m_Window;
		EventCallbackFn m_EventCallback;
	};

}
