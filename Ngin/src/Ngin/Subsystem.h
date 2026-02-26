#pragma once

#include "Core.h"
#include "Events/Event.h"

namespace Ngin {

	class NG_API Subsystem
	{
	public:
		virtual ~Subsystem() = default;

		virtual void OnInit() {}
		virtual void OnShutdown() {}
		virtual void Tick(float deltaTime) {}
		virtual void OnEvent(Event& event) {}
	};

}
