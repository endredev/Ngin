#include <Ngin.h>

class Sandbox : public Ngin::Application
{
public:
	Sandbox() {

	}

	~Sandbox() {

	}
};

Ngin::Application* Ngin::CreateApplication() {
	return new Sandbox();
}