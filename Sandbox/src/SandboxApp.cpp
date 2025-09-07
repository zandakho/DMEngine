
#include "Sandbox2D.h"

namespace DME
{
	class SandboxApp : public Application
	{
	public:
		SandboxApp(const ApplicationSpecification& specification)
			: Application(specification)
		{
			PushLayer(new Sandbox2D());
		}

		~SandboxApp()
		{

		}
	};

	Application* CreateApplication(ApplicationCommandLineArgs args)
	{
		ApplicationSpecification spec;
		spec.Name = "SandboxApp";
		spec.WorkingDirectory = "../DMEEditor";
		spec.CommandLineArgs = args;

		return new SandboxApp(spec);
	}
}
