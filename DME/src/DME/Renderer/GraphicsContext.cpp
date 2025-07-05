#include "dmepch.h"

// --- DME/Renderer/ -----------------------------------------//
#include "DME/Renderer/GraphicsContext.h"						//
#include "DME/Renderer/Renderer.h"							//
// -------------------------------------------------------------//

// --- Platform/OpenGL/ ----------------------------------------//
#include "Platform/OpenGL/OpenGLContext.h"						//
// -------------------------------------------------------------//

namespace DME {

	Scope<GraphicsContext> GraphicsContext::Create(void* window)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    DME_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateScope<OpenGLContext>(static_cast<GLFWwindow*>(window));
		}

		DME_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}