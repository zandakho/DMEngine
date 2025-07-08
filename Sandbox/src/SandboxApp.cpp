
#include "Sandbox2D.h"

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
