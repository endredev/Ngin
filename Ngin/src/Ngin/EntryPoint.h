#pragma once

#ifdef NG_PLATFORM_WINDOWS

extern Ngin::Application* Ngin::CreateApplication();

int main(int argc, char** argv)
{
	Ngin::Log::Init();
	NG_WARN("Initialized");
	NG_TRACE("TRACE");
	NG_ERROR("Error");
	int b = 8;
	NG_INFO("Hello, var = {0}", b);

	auto app = Ngin::CreateApplication();
	app->Run();
	delete app;
}

#endif