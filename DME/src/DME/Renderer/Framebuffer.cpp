
#include "dmepch.h"

#include "DME/Renderer/Renderer.h"
#include "DME/Renderer/Framebuffer.h"

#include "Platform/OpenGL/OpenGLFramebuffer.h"

namespace DME
{

	Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& specification)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:		DME_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:		return CreateRef<OpenGLFramebuffer>(specification);
		}

		DME_CORE_ASSERT(false, "Unknown Specification!");
		return nullptr;
	}

}