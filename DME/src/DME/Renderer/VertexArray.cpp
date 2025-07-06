#include "dmepch.h"

#include "DME/Renderer/Renderer.h"
#include "DME/Renderer/VertexArray.h"

#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace DME
{
	Ref<VertexArray> VertexArray::Create()
	{

		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:		DME_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:		return CreateRef<OpenGLVertexArray>();
		}

		DME_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}