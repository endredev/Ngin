#include "ngpch.h"
#include "OpenGLShader.h"

#include <glad/gl.h>

namespace Ngin {

	Shader* Shader::Create(const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		return new OpenGLShader(vertexSrc, fragmentSrc);
	}

	OpenGLShader::OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc)
		: m_RendererID(0)
	{
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		const GLchar* src = vertexSrc.c_str();
		glShaderSource(vertexShader, 1, &src, nullptr);
		glCompileShader(vertexShader);

		GLint isCompiled = 0;
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);
			std::vector<GLchar> log(maxLength);
			glGetShaderInfoLog(vertexShader, maxLength, &maxLength, log.data());
			glDeleteShader(vertexShader);
			NG_CORE_ERROR("Vertex shader compile error: {0}", log.data());
			return;
		}

		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		src = fragmentSrc.c_str();
		glShaderSource(fragmentShader, 1, &src, nullptr);
		glCompileShader(fragmentShader);

		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);
			std::vector<GLchar> log(maxLength);
			glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, log.data());
			glDeleteShader(fragmentShader);
			glDeleteShader(vertexShader);
			NG_CORE_ERROR("Fragment shader compile error: {0}", log.data());
			return;
		}

		m_RendererID = glCreateProgram();
		glAttachShader(m_RendererID, vertexShader);
		glAttachShader(m_RendererID, fragmentShader);
		glLinkProgram(m_RendererID);

		GLint isLinked = 0;
		glGetProgramiv(m_RendererID, GL_LINK_STATUS, &isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(m_RendererID, GL_INFO_LOG_LENGTH, &maxLength);
			std::vector<GLchar> log(maxLength);
			glGetProgramInfoLog(m_RendererID, maxLength, &maxLength, log.data());
			glDeleteProgram(m_RendererID);
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);
			NG_CORE_ERROR("Shader link error: {0}", log.data());
			return;
		}

		glDetachShader(m_RendererID, vertexShader);
		glDetachShader(m_RendererID, fragmentShader);
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}

	OpenGLShader::~OpenGLShader()
	{
		glDeleteProgram(m_RendererID);
	}

	void OpenGLShader::Bind() const
	{
		glUseProgram(m_RendererID);
	}

	void OpenGLShader::Unbind() const
	{
		glUseProgram(0);
	}

	void OpenGLShader::SetMat4(const std::string& name, const float* matrix)
	{
		GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, matrix);
	}

}
