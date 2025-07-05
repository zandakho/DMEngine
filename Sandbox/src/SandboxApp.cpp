
// --- Dependencies --------------------------------------------//
#include <dme.h>												//
#include <ImGui/imgui.h>										//
#include <glm/gtc/type_ptr.hpp>									//
#include <glm/gtc/matrix_transform.hpp>							//
// -------------------------------------------------------------//

// --- DME/Core/ ---------------------------------------------//
#include <DME/Core/EntryPoint.h>								//
// -------------------------------------------------------------//

// --- Platform/OpenGL/ ----------------------------------------//
#include "Platform/OpenGL/OpenGLShader.h"						//
// -------------------------------------------------------------//

// --- src -----------------------------------------------------//
#include "Sandbox2D.h"											//
#include "ExampleLayer.h"										//
// -------------------------------------------------------------//

namespace DME
{
	class SandboxApp : public Application
	{
	public:
		SandboxApp()
		{
			PushLayer(new Sandbox2D());
		}

		~SandboxApp()
		{

		}
	};

	Application* CreateApplication()
	{
		return new SandboxApp();
	}
}
