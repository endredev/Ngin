#pragma once

#include "Ngin/Renderer/Shader.h"

namespace Ngin {

	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc);
		virtual ~OpenGLShader();

		void Bind() const override;
		void Unbind() const override;

	private:
		uint32_t m_RendererID;
	};

}
