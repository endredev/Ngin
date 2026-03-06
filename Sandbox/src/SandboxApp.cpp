#include <Ngin.h>

#include <memory>
#include <cmath>

static const char* s_CubeVertSrc = R"(
	#version 330 core
	layout(location = 0) in vec3 a_Position;
	layout(location = 1) in vec4 a_Color;

	uniform mat4 u_ViewProjection;
	uniform mat4 u_Model;

	out vec4 v_Color;

	void main()
	{
		v_Color = a_Color;
		gl_Position = u_ViewProjection * u_Model * vec4(a_Position, 1.0);
	}
)";

static const char* s_CubeFragSrc = R"(
	#version 330 core
	in vec4 v_Color;
	out vec4 color;
	void main()
	{
		color = v_Color;
	}
)";

class Sandbox : public Ngin::Application
{
public:
	Sandbox()
	{
		m_Camera2D = std::make_unique<Ngin::OrthographicCamera>(-1.6f, 1.6f, -0.9f, 0.9f);

		// 3D camera: 45 deg fov, 16:9, near=0.1, far=100
		m_Camera3D = std::make_unique<Ngin::PerspectiveCamera>(
			0.785398f, 1280.0f / 720.0f, 0.1f, 100.0f
		);
		m_Camera3D->SetPosition(0.0f, 0.0f, 3.0f);

		// Cube: 8 vertices, per-vertex color
		float vertices[] = {
			// Position              // Color (RGBA)
			-0.5f, -0.5f, -0.5f,    0.9f, 0.2f, 0.2f, 1.0f,
			 0.5f, -0.5f, -0.5f,    0.2f, 0.9f, 0.2f, 1.0f,
			 0.5f,  0.5f, -0.5f,    0.2f, 0.2f, 0.9f, 1.0f,
			-0.5f,  0.5f, -0.5f,    0.9f, 0.9f, 0.2f, 1.0f,
			-0.5f, -0.5f,  0.5f,    0.9f, 0.2f, 0.9f, 1.0f,
			 0.5f, -0.5f,  0.5f,    0.2f, 0.9f, 0.9f, 1.0f,
			 0.5f,  0.5f,  0.5f,    0.9f, 0.9f, 0.9f, 1.0f,
			-0.5f,  0.5f,  0.5f,    0.4f, 0.4f, 0.4f, 1.0f,
		};

		uint32_t indices[] = {
			0, 1, 2,  2, 3, 0,  // back
			4, 5, 6,  6, 7, 4,  // front
			0, 4, 7,  7, 3, 0,  // left
			1, 5, 6,  6, 2, 1,  // right
			3, 2, 6,  6, 7, 3,  // top
			0, 1, 5,  5, 4, 0,  // bottom
		};

		m_CubeVA.reset(Ngin::VertexArray::Create());

		auto vb = std::shared_ptr<Ngin::VertexBuffer>(
			Ngin::VertexBuffer::Create(vertices, sizeof(vertices))
		);
		vb->SetLayout({
			{ Ngin::ShaderDataType::Float3, "a_Position" },
			{ Ngin::ShaderDataType::Float4, "a_Color"    }
		});
		m_CubeVA->AddVertexBuffer(vb);

		auto ib = std::shared_ptr<Ngin::IndexBuffer>(
			Ngin::IndexBuffer::Create(indices, 36)
		);
		m_CubeVA->SetIndexBuffer(ib);

		m_CubeShader.reset(Ngin::Shader::Create(s_CubeVertSrc, s_CubeFragSrc));
	}

	void Tick(float deltaTime) override
	{
		m_Rotation += deltaTime;

		// 3D cube
		Ngin::Renderer::BeginScene(*m_Camera3D);

		Ngin::Mat4 model = Ngin::Mat4::RotateY(m_Rotation) * Ngin::Mat4::RotateX(m_Rotation * 0.5f);
		Ngin::Renderer::Submit(m_CubeVA.get(), m_CubeShader.get(), model.data);
		Ngin::Renderer::EndScene();

		// 2D squares
		Ngin::Renderer2D::BeginScene(*m_Camera2D);
		Ngin::Renderer2D::DrawQuad(-1.3f,  0.7f, 0.2f, 0.2f,  0.8f, 0.2f, 0.2f);
		Ngin::Renderer2D::DrawQuad(-1.3f,  0.4f, 0.2f, 0.2f,  0.2f, 0.8f, 0.2f);
		Ngin::Renderer2D::EndScene();
	}

private:
	// 2D
	std::unique_ptr<Ngin::OrthographicCamera> m_Camera2D;

	// 3D
	std::unique_ptr<Ngin::PerspectiveCamera> m_Camera3D;
	std::unique_ptr<Ngin::VertexArray>       m_CubeVA;
	std::unique_ptr<Ngin::Shader>            m_CubeShader;
	float                                    m_Rotation = 0.0f;
};

Ngin::Application* Ngin::CreateApplication()
{
	return new Sandbox();
}
