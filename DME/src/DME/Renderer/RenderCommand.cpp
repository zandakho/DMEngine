#include "dmepch.h"

#include "DME/Renderer/RenderCommand.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace DME
{
	Scope<RendererAPI> RenderCommand::s_RendererAPI = RendererAPI::Create();
}