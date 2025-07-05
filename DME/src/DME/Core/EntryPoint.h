#pragma once

// --- DME/Core/ ---------------------------------------------//
#include "DME/Core/Base.h"									//
// -------------------------------------------------------------//

#ifdef DME_PLATFORM_WINDOWS

extern DME::Application* DME::CreateApplication();

int main(int argc, char** argv)
{
	DME::Log::Init();

	HZ_PROFILE_BEGIN_SESSION("Startup", "Profiling/Profile-Startup.json");
	auto app = DME::CreateApplication();
	HZ_PROFILE_END_SESSION();

	HZ_PROFILE_BEGIN_SESSION("Runtime", "Profiling/Profile-Runtime.json");
	app->Run();
	HZ_PROFILE_END_SESSION();

	HZ_PROFILE_BEGIN_SESSION("Shutdown", "Profiling/Profile-Shutdown.json");
	delete app;
	HZ_PROFILE_END_SESSION();

}

#endif
