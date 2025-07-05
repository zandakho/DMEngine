#include "dmepch.h"


#include "DME/Core/Window.h"

#ifdef DME_PLATFORM_WINDOWS
#include "Platform/Windows/WindowsWindow.h"
#endif

namespace DME
{
	Scope<Window> Window::Create(const WindowProps& props)
	{
#ifdef DME_PLATFORM_WINDOWS
		return CreateScope<WindowsWindow>(props);
#else
		HZ_CORE_ASSERT(false, "Unknown platform!");
		return nullptr;
#endif
	}

}