#include "ngpch.h"

#include "InputSubsystem.h"
#include "Ngin/Events/KeyEvent.h"
#include "Ngin/Events/MouseEvent.h"

namespace Ngin {

	void InputSubsystem::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);

		dispatcher.Dispatch<KeyPressedEvent>([this](KeyPressedEvent& e)
		{
			m_KeyStates[e.GetKeyCode()] = true;
			return false;
		});

		dispatcher.Dispatch<KeyReleasedEvent>([this](KeyReleasedEvent& e)
		{
			m_KeyStates[e.GetKeyCode()] = false;
			return false;
		});

		dispatcher.Dispatch<MouseButtonPressedEvent>([this](MouseButtonPressedEvent& e)
		{
			m_MouseButtonStates[e.GetMouseButton()] = true;
			return false;
		});

		dispatcher.Dispatch<MouseButtonReleasedEvent>([this](MouseButtonReleasedEvent& e)
		{
			m_MouseButtonStates[e.GetMouseButton()] = false;
			return false;
		});

		dispatcher.Dispatch<MouseMovedEvent>([this](MouseMovedEvent& e)
		{
			m_MouseX = e.GetX();
			m_MouseY = e.GetY();
			return false;
		});
	}

	bool InputSubsystem::IsKeyPressed(Key keycode) const
	{
		auto it = m_KeyStates.find((int)keycode);
		return it != m_KeyStates.end() && it->second;
	}

	bool InputSubsystem::IsMouseButtonPressed(Mouse button) const
	{
		auto it = m_MouseButtonStates.find((int)button);
		return it != m_MouseButtonStates.end() && it->second;
	}

}
