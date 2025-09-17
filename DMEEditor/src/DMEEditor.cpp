#include <dme.h>

#include "EditorLayer.h"

#include <DME/Core/EntryPoint.h>

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


