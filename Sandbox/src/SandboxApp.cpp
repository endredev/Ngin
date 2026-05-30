#include <Ngin.h>
#include "EditorStyle.h"

#include <memory>
#include <cmath>


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

		// Cube: 24 vertices (4 per face), per-face normals
		// Layout: Position(xyz), Normal(xyz)
		float vertices[] = {
			// Front  (z=+0.5, n=0,0,+1)
			-0.5f,-0.5f, 0.5f,   0.0f, 0.0f, 1.0f,
			 0.5f,-0.5f, 0.5f,   0.0f, 0.0f, 1.0f,
			 0.5f, 0.5f, 0.5f,   0.0f, 0.0f, 1.0f,
			-0.5f, 0.5f, 0.5f,   0.0f, 0.0f, 1.0f,
			// Back   (z=-0.5, n=0,0,-1)
			-0.5f,-0.5f,-0.5f,   0.0f, 0.0f,-1.0f,
			-0.5f, 0.5f,-0.5f,   0.0f, 0.0f,-1.0f,
			 0.5f, 0.5f,-0.5f,   0.0f, 0.0f,-1.0f,
			 0.5f,-0.5f,-0.5f,   0.0f, 0.0f,-1.0f,
			// Right  (x=+0.5, n=+1,0,0)
			 0.5f,-0.5f, 0.5f,   1.0f, 0.0f, 0.0f,
			 0.5f,-0.5f,-0.5f,   1.0f, 0.0f, 0.0f,
			 0.5f, 0.5f,-0.5f,   1.0f, 0.0f, 0.0f,
			 0.5f, 0.5f, 0.5f,   1.0f, 0.0f, 0.0f,
			// Left   (x=-0.5, n=-1,0,0)
			-0.5f,-0.5f,-0.5f,  -1.0f, 0.0f, 0.0f,
			-0.5f,-0.5f, 0.5f,  -1.0f, 0.0f, 0.0f,
			-0.5f, 0.5f, 0.5f,  -1.0f, 0.0f, 0.0f,
			-0.5f, 0.5f,-0.5f,  -1.0f, 0.0f, 0.0f,
			// Top    (y=+0.5, n=0,+1,0)
			 0.5f, 0.5f,-0.5f,   0.0f, 1.0f, 0.0f,
			-0.5f, 0.5f,-0.5f,   0.0f, 1.0f, 0.0f,
			-0.5f, 0.5f, 0.5f,   0.0f, 1.0f, 0.0f,
			 0.5f, 0.5f, 0.5f,   0.0f, 1.0f, 0.0f,
			// Bottom (y=-0.5, n=0,-1,0)
			-0.5f,-0.5f,-0.5f,   0.0f,-1.0f, 0.0f,
			 0.5f,-0.5f,-0.5f,   0.0f,-1.0f, 0.0f,
			 0.5f,-0.5f, 0.5f,   0.0f,-1.0f, 0.0f,
			-0.5f,-0.5f, 0.5f,   0.0f,-1.0f, 0.0f,
		};

		uint32_t indices[] = {
			 0, 1, 2,  2, 3, 0,   // front
			 4, 5, 6,  6, 7, 4,   // back
			 8, 9,10, 10,11, 8,   // right
			12,13,14, 14,15,12,   // left
			16,17,18, 18,19,16,   // top
			20,21,22, 22,23,20,   // bottom
		};

		m_CubeVA.reset(Ngin::VertexArray::Create());

		auto vb = std::shared_ptr<Ngin::VertexBuffer>(
			Ngin::VertexBuffer::Create(vertices, sizeof(vertices))
		);
		vb->SetLayout({
			{ Ngin::ShaderDataType::Float3, "a_Position" },
			{ Ngin::ShaderDataType::Float3, "a_Normal"   }
		});
		m_CubeVA->AddVertexBuffer(vb);

		auto ib = std::shared_ptr<Ngin::IndexBuffer>(
			Ngin::IndexBuffer::Create(indices, 36)
		);
		m_CubeVA->SetIndexBuffer(ib);

	}

	void OnImGuiStyle() override
	{
		EditorStyle::Apply(m_Style);
	}

	void OnViewportResize(uint32_t width, uint32_t height) override
	{
		m_Camera3D->SetAspectRatio((float)width / (float)height);
	}

	void OnPropertiesPanel() override
	{
		ImGui::Text("Rotation: %.2f rad", m_Rotation);
	}

	void Tick(float deltaTime) override
	{
		m_Rotation += deltaTime;

		// 3D cube
		Ngin::Renderer::BeginScene(*m_Camera3D);

		Ngin::Mat4 model = Ngin::Mat4::RotateY(m_Rotation) * Ngin::Mat4::RotateX(m_Rotation * 0.5f);
		Ngin::Renderer::Submit(m_CubeVA.get(), Ngin::Renderer::GetDefaultShader(), model.data);
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
	std::unique_ptr<Ngin::VertexArray> m_CubeVA;
	float                              m_Rotation = 0.0f;
	EditorStyle                              m_Style;
};

Ngin::Application* Ngin::CreateApplication()
{
	return new Sandbox();
}
