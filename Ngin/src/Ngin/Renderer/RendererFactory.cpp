#include "ngpch.h"

#include "Buffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Framebuffer.h"
#include "Platforms/DirectX12/DX12Buffer.h"
#include "Platforms/DirectX12/DX12VertexArray.h"
#include "Platforms/DirectX12/DX12Shader.h"
#include "Platforms/DirectX12/DX12Framebuffer.h"

namespace Ngin {

	VertexBuffer* VertexBuffer::Create(float* vertices, uint32_t size)
	{
		return new DX12VertexBuffer(vertices, size);
	}

	VertexBuffer* VertexBuffer::Create(uint32_t size)
	{
		return new DX12VertexBuffer(size);
	}

	IndexBuffer* IndexBuffer::Create(uint32_t* indices, uint32_t count)
	{
		return new DX12IndexBuffer(indices, count);
	}

	VertexArray* VertexArray::Create()
	{
		return new DX12VertexArray();
	}

	Shader* Shader::Create(const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		return new DX12Shader(vertexSrc, fragmentSrc);
	}

	Framebuffer* Framebuffer::Create(uint32_t width, uint32_t height)
	{
		return new DX12Framebuffer(width, height);
	}

}
