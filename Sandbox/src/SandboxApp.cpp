#include <Ngin.h>

#include <memory>

class Sandbox : public Ngin::Application
{
public:
	Sandbox()
	{
		Ngin::Renderer2D::Init();
		m_Camera = std::make_unique<Ngin::OrthographicCamera>(-1.6f, 1.6f, -0.9f, 0.9f);
	}

	~Sandbox()
	{
		Ngin::Renderer2D::Shutdown();
	}

	void Tick(float deltaTime) override
	{
		Ngin::Renderer2D::BeginScene(*m_Camera);

		Ngin::Renderer2D::DrawQuad(-0.5f,  0.0f, 0.8f, 0.8f,  0.8f, 0.2f, 0.2f); // piros
		Ngin::Renderer2D::DrawQuad( 0.5f,  0.0f, 0.6f, 0.6f,  0.2f, 0.8f, 0.2f); // zöld
		Ngin::Renderer2D::DrawQuad( 0.0f, -0.4f, 0.4f, 0.4f,  0.2f, 0.2f, 0.8f); // kék

		Ngin::Renderer2D::EndScene();
	}

private:
	std::unique_ptr<Ngin::OrthographicCamera> m_Camera;
};

Ngin::Application* Ngin::CreateApplication()
{
	return new Sandbox();
}
