#include "dmepch.h"

// --- DME/Renderer/ -----------------------------------------//
#include "DME/Renderer/RenderCommand.h"						//
// -------------------------------------------------------------//

// --- Platform/OpenGL/ ----------------------------------------//
#include "Platform/OpenGL/OpenGLRendererAPI.h"					//
// -------------------------------------------------------------//

namespace DME
{
	Scope<RendererAPI> RenderCommand::s_RendererAPI = RendererAPI::Create();
}