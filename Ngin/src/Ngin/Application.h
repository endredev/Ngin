#pragma once
#include "Core.h"

namespace Ngin {
	class NG_API Application
	{
	public: 
		Application();
		virtual ~Application();
		void Run();
	};

	// To be defined in client
	Application* CreateApplication();
}

