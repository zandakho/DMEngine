#pragma once

#include "DME/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace DME
{
	class OpenGLContext : public GraphicsContext
	{
	public:
		OpenGLContext(GLFWwindow* windowHandle);

		void Init() override;
		void SwapBuffers() override;

	private:
		GLFWwindow* m_WindowHandle;
	};
}