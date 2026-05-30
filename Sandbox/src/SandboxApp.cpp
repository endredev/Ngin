#include <Ngin.h>
#include "EditorStyle.h"

#include <memory>
#include <cmath>

static const char* s_CubeVertSrc = R"(
	cbuffer Constants : register(b0)
	{
		column_major float4x4 u_ViewProjection;
		column_major float4x4 u_Model;
	};
	struct VSInput { float3 Position : TEXCOORD0; float4 Color : TEXCOORD1; };
	struct PSInput { float4 Position : SV_POSITION; float4 Color : TEXCOORD0; };
	PSInput main(VSInput input)
	{
		PSInput output;
		output.Position = mul(u_ViewProjection, mul(u_Model, float4(input.Position, 1.0)));
		output.Color    = input.Color;
		return output;
	}
)";
static const char* s_CubeFragSrc = R"(
	struct PSInput { float4 Position : SV_POSITION; float4 Color : TEXCOORD0; };
	float4 main(PSInput input) : SV_TARGET { return input.Color; }
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
			2, 1, 0,  0, 3, 2,  // back
			4, 5, 6,  6, 7, 4,  // front
			0, 4, 7,  7, 3, 0,  // left
			6, 5, 1,  1, 2, 6,  // right
			6, 2, 3,  3, 7, 6,  // top
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
	EditorStyle                              m_Style;
};

Ngin::Application* Ngin::CreateApplication()
{
	return new Sandbox();
}
