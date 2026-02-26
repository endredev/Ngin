#include <Ngin.h>

class Sandbox : public Ngin::Application
{
public:
	Sandbox() {}
	~Sandbox() {}

	void Tick(float deltaTime) override
	{
		auto* input = GetSubsystemManager().Get<Ngin::InputSubsystem>();

		if (input->IsKeyPressed(Ngin::Key::W))
			NG_INFO("Move Forward");

		if (input->IsKeyPressed(Ngin::Key::S))
			NG_INFO("Move Backward");

		if (input->IsKeyPressed(Ngin::Key::A))
			NG_INFO("Move Left");

		if (input->IsKeyPressed(Ngin::Key::D))
			NG_INFO("Move Right");
	}
};

Ngin::Application* Ngin::CreateApplication()
{
	return new Sandbox();
}
