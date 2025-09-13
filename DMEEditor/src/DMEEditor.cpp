#include <dme.h>

#include "EditorLayer.h"
#include "Platform/OpenGL/OpenGLShader.h"

#include <ImGui/imgui.h>
#include <DME/Core/EntryPoint.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

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


