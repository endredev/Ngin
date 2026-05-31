#include <Ngin.h>

#include <memory>

class Sandbox : public Ngin::Application
{
public:
	Sandbox()
	{
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
			 0, 1, 2,  2, 3, 0,
			 4, 5, 6,  6, 7, 4,
			 8, 9,10, 10,11, 8,
			12,13,14, 14,15,12,
			16,17,18, 18,19,16,
			20,21,22, 22,23,20,
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

	void OnPropertiesPanel() override
	{
		ImGui::Text("Rotation: %.2f rad", m_Rotation);
	}

	void Tick(float deltaTime) override
	{
		m_Rotation += deltaTime;
		Ngin::Mat4 model = Ngin::Mat4::RotateY(m_Rotation) * Ngin::Mat4::RotateX(m_Rotation * 0.5f);
		Ngin::Renderer::Submit(m_CubeVA.get(), Ngin::Renderer::GetDefaultShader(), model.data);
	}

private:
	std::unique_ptr<Ngin::VertexArray> m_CubeVA;
	float                              m_Rotation = 0.0f;
};

Ngin::Application* Ngin::CreateApplication()
{
	return new Sandbox();
}
