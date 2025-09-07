
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
#include "EditorLayer.h"										//
// -------------------------------------------------------------//

namespace DME
{
	class DMEEditor : public Application
	{
	public:
		DMEEditor(const ApplicationSpecification& spec) : Application(spec)
		{
			PushLayer(new EditorLayer());
		}
	};

	Application* CreateApplication(ApplicationCommandLineArgs args)
	{
		ApplicationSpecification spec;
		spec.Name = "DMEEditor";
		spec.CommandLineArgs = args;

		return new DMEEditor(spec);
	}
}


