#pragma once

#include "Ngin/Core.h"

namespace Ngin {

	class NG_API Framebuffer
	{
	public:
		virtual ~Framebuffer() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual void Resize(uint32_t width, uint32_t height) = 0;

		virtual uint32_t GetColorAttachmentID() const = 0;
		virtual uint32_t GetWidth()  const = 0;
		virtual uint32_t GetHeight() const = 0;

		static Framebuffer* Create(uint32_t width, uint32_t height);
	};

}
