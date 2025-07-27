#include "dmepch.h"
#include "DME/Renderer/DebugRendererMode.h"

namespace DME
{
	DebugRendererMode s_DebugRendererMode = DebugRendererMode::Normal;

	std::string DebugModeToString(DebugRendererMode mode)
	{
		{
			switch (mode)
			{
			case DebugRendererMode::Normal:			return "Normal";
			case DebugRendererMode::Wireframe:		return "Wireframe";
			case DebugRendererMode::Point:			return "Point";
			}

			DME_CORE_ASSERT(false, "Unknown debug mode!");
			return std::string();
		}
	}
}