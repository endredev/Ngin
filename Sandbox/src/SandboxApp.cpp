#include <Ngin.h>

#include <memory>

class Sandbox : public Ngin::Application
{
public:
	Sandbox()
	{
		// Vertex data: position (xyz) + color (rgb)
		float vertices[] = {
			-0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,
			 0.0f,  0.5f, 0.0f,   0.0f, 0.0f, 1.0f,
		};

		uint32_t indices[] = { 0, 1, 2 };

		m_VertexArray.reset(Ngin::VertexArray::Create());

		auto vb = std::shared_ptr<Ngin::VertexBuffer>(
			Ngin::VertexBuffer::Create(vertices, sizeof(vertices))
		);
		vb->SetLayout({
			{ Ngin::ShaderDataType::Float3, "a_Position" },
			{ Ngin::ShaderDataType::Float3, "a_Color"    }
		});
		m_VertexArray->AddVertexBuffer(vb);

		auto ib = std::shared_ptr<Ngin::IndexBuffer>(
			Ngin::IndexBuffer::Create(indices, 3)
		);
		m_VertexArray->SetIndexBuffer(ib);

		std::string vertSrc = R"(
			#version 330 core
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec3 a_Color;
			out vec3 v_Color;
			void main()
			{
				v_Color = a_Color;
				gl_Position = vec4(a_Position, 1.0);
			}
		)";

		std::string fragSrc = R"(
			#version 330 core
			in vec3 v_Color;
			out vec4 color;
			void main()
			{
				color = vec4(v_Color, 1.0);
			}
		)";

		m_Shader.reset(Ngin::Shader::Create(vertSrc, fragSrc));
	}

	~Sandbox() {}

	void Tick(float deltaTime) override
	{
		auto* renderer = GetSubsystemManager().Get<Ngin::RendererSubsystem>();
		renderer->Submit(m_VertexArray.get(), m_Shader.get());

		auto* input = GetSubsystemManager().Get<Ngin::InputSubsystem>();
		if (input->IsKeyPressed(Ngin::Key::Escape))
			NG_INFO("Escape pressed");
	}

private:
	std::unique_ptr<Ngin::VertexArray> m_VertexArray;
	std::unique_ptr<Ngin::Shader>      m_Shader;
};

Ngin::Application* Ngin::CreateApplication()
{
	return new Sandbox();
}
