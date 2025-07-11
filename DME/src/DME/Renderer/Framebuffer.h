#pragma once

#include "DME/Core/Base.h"

namespace DME
{

	struct FramebufferSpecification
	{
		uint32_t Width = 1, Height = 1;
		// FramebufferFormat Format = ;
		uint32_t Samples = 1; 

		bool SwapChainTarget = false;
	};

	class Framebuffer
	{
	public:

		virtual ~Framebuffer() = default;

		virtual void Bind() = 0;
		virtual void UnBind() = 0;

		virtual void Resize(uint32_t width = 0, uint32_t height = 0) = 0;

		virtual uint32_t GetColorAttachmentRendererID() const = 0;

		virtual const FramebufferSpecification& GetSpecification() const = 0;

		static Ref<Framebuffer> Create(const FramebufferSpecification& specification);
	};
}