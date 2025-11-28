#pragma once

#ifdef NG_PLATFORM_WINDOWS

extern Ngin::Application* Ngin::CreateApplication();

int main(int argc, char** argv)
{
	auto app = Ngin::CreateApplication();
	app->Run();
	delete app;
}

#endif