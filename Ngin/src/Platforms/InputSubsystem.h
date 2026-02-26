#pragma once

#include "Ngin/Subsystem.h"
#include "Ngin/KeyCodes.h"
#include "Ngin/MouseCodes.h"

#include <unordered_map>
#include <utility>

namespace Ngin {

	class NG_API InputSubsystem : public Subsystem
	{
	public:
		void OnEvent(Event& event) override;

		bool IsKeyPressed(Key keycode) const;
		bool IsMouseButtonPressed(Mouse button) const;

		float GetMouseX() const { return m_MouseX; }
		float GetMouseY() const { return m_MouseY; }
		std::pair<float, float> GetMousePos() const { return { m_MouseX, m_MouseY }; }

	private:
		std::unordered_map<int, bool> m_KeyStates;
		std::unordered_map<int, bool> m_MouseButtonStates;
		float m_MouseX = 0.0f;
		float m_MouseY = 0.0f;
	};

}
