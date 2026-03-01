#include "ngpch.h"
#include "Renderer2D.h"

#include "Buffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "RenderCommand.h"

#include <memory>

namespace Ngin {

	struct QuadVertex
	{
		float Position[3];
		float Color[4];
	};

	static const uint32_t MaxQuads    = 10000;
	static const uint32_t MaxVertices = MaxQuads * 4;
	static const uint32_t MaxIndices  = MaxQuads * 6;

	struct Renderer2DData
	{
		VertexArray*  VAO              = nullptr;
		VertexBuffer* VBO              = nullptr;
		IndexBuffer*  IBO              = nullptr;
		Shader*       BatchShader      = nullptr;

		QuadVertex*   VertexBufferBase = nullptr;
		QuadVertex*   VertexBufferPtr  = nullptr;
		uint32_t      IndexCount       = 0;
	};

	static Renderer2DData s_Data;

	static const char* s_VertexSrc = R"(
		#version 330 core
		layout(location = 0) in vec3 a_Position;
		layout(location = 1) in vec4 a_Color;
		uniform mat4 u_ViewProjection;
		out vec4 v_Color;
		void main()
		{
			v_Color = a_Color;
			gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
		}
	)";

	static const char* s_FragmentSrc = R"(
		#version 330 core
		in vec4 v_Color;
		out vec4 color;
		void main()
		{
			color = v_Color;
		}
	)";

	void Renderer2D::Init()
	{
		s_Data.VAO = VertexArray::Create();

		s_Data.VBO = VertexBuffer::Create(MaxVertices * (uint32_t)sizeof(QuadVertex));
		s_Data.VBO->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float4, "a_Color"    }
		});
		s_Data.VAO->AddVertexBuffer(std::shared_ptr<VertexBuffer>(s_Data.VBO));

		// Pre-compute index buffer (0,1,2,2,3,0 pattern per quad)
		uint32_t* indices = new uint32_t[MaxIndices];
		uint32_t offset = 0;
		for (uint32_t i = 0; i < MaxIndices; i += 6)
		{
			indices[i + 0] = offset + 0;
			indices[i + 1] = offset + 1;
			indices[i + 2] = offset + 2;
			indices[i + 3] = offset + 2;
			indices[i + 4] = offset + 3;
			indices[i + 5] = offset + 0;
			offset += 4;
		}
		s_Data.IBO = IndexBuffer::Create(indices, MaxIndices);
		s_Data.VAO->SetIndexBuffer(std::shared_ptr<IndexBuffer>(s_Data.IBO));
		delete[] indices;

		s_Data.BatchShader = Shader::Create(s_VertexSrc, s_FragmentSrc);

		s_Data.VertexBufferBase = new QuadVertex[MaxVertices];
	}

	void Renderer2D::Shutdown()
	{
		delete[] s_Data.VertexBufferBase;
		delete s_Data.BatchShader;
		delete s_Data.VAO;
		// VBO and IBO are owned by VAO's shared_ptrs — already deleted
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		s_Data.VertexBufferPtr = s_Data.VertexBufferBase;
		s_Data.IndexCount = 0;

		s_Data.BatchShader->Bind();
		s_Data.BatchShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
	}

	void Renderer2D::DrawQuad(float x, float y, float width, float height,
	                          float r, float g, float b, float a)
	{
		float halfW = width  * 0.5f;
		float halfH = height * 0.5f;

		// Bottom-left
		s_Data.VertexBufferPtr->Position[0] = x - halfW;
		s_Data.VertexBufferPtr->Position[1] = y - halfH;
		s_Data.VertexBufferPtr->Position[2] = 0.0f;
		s_Data.VertexBufferPtr->Color[0] = r;
		s_Data.VertexBufferPtr->Color[1] = g;
		s_Data.VertexBufferPtr->Color[2] = b;
		s_Data.VertexBufferPtr->Color[3] = a;
		s_Data.VertexBufferPtr++;

		// Bottom-right
		s_Data.VertexBufferPtr->Position[0] = x + halfW;
		s_Data.VertexBufferPtr->Position[1] = y - halfH;
		s_Data.VertexBufferPtr->Position[2] = 0.0f;
		s_Data.VertexBufferPtr->Color[0] = r;
		s_Data.VertexBufferPtr->Color[1] = g;
		s_Data.VertexBufferPtr->Color[2] = b;
		s_Data.VertexBufferPtr->Color[3] = a;
		s_Data.VertexBufferPtr++;

		// Top-right
		s_Data.VertexBufferPtr->Position[0] = x + halfW;
		s_Data.VertexBufferPtr->Position[1] = y + halfH;
		s_Data.VertexBufferPtr->Position[2] = 0.0f;
		s_Data.VertexBufferPtr->Color[0] = r;
		s_Data.VertexBufferPtr->Color[1] = g;
		s_Data.VertexBufferPtr->Color[2] = b;
		s_Data.VertexBufferPtr->Color[3] = a;
		s_Data.VertexBufferPtr++;

		// Top-left
		s_Data.VertexBufferPtr->Position[0] = x - halfW;
		s_Data.VertexBufferPtr->Position[1] = y + halfH;
		s_Data.VertexBufferPtr->Position[2] = 0.0f;
		s_Data.VertexBufferPtr->Color[0] = r;
		s_Data.VertexBufferPtr->Color[1] = g;
		s_Data.VertexBufferPtr->Color[2] = b;
		s_Data.VertexBufferPtr->Color[3] = a;
		s_Data.VertexBufferPtr++;

		s_Data.IndexCount += 6;
	}

	void Renderer2D::EndScene()
	{
		Flush();
	}

	void Renderer2D::Flush()
	{
		if (s_Data.IndexCount == 0)
			return;

		uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.VertexBufferPtr - (uint8_t*)s_Data.VertexBufferBase);
		s_Data.VBO->SetData(s_Data.VertexBufferBase, dataSize);

		RenderCommand::DrawIndexed(s_Data.VAO, s_Data.IndexCount);
	}

}
