#pragma once

#include "DME/Debug/Instrumentator.h"
#include "DME/Core/Application.h"

#ifdef DME_PLATFORM_WINDOWS

extern DME::Application* DME::CreateApplication(ApplicationCommandLineArgs args);

int main(int argc, char** argv)
{
	DME::Log::Init();

	DME_PROFILE_BEGIN_SESSION("Startup", "Profiling/Profile-Startup.json");
	auto app = DME::CreateApplication({ argc, argv });
	DME_PROFILE_END_SESSION();

	DME_PROFILE_BEGIN_SESSION("Runtime", "Profiling/Profile-Runtime.json");
	app->Run();
	DME_PROFILE_END_SESSION();

	DME_PROFILE_BEGIN_SESSION("Shutdown", "Profiling/Profile-Shutdown.json");
	delete app;
	DME_PROFILE_END_SESSION();

}

#endif
