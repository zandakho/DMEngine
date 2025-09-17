#include "dmepch.h"
#include "DME/Renderer/DebugRendererMode.h"

#include "DME/Core/KeyCodes.h"

namespace DME
{

	DebugRendererMode s_DebugRendererMode = DebugRendererMode::Normal;

	DebugRenderer::DebugRenderer() {}

	std::string DebugRenderer::DebugModeToString(DebugRendererMode mode)
	{
		{
			switch (mode)
			{
				case DebugRendererMode::Normal:			return "Normal";
				case DebugRendererMode::Wireframe:		return "Wireframe (Lines)";
				case DebugRendererMode::Point:			return "Point (Points)";
			}

			DME_CORE_ASSERT(false, "Unknown debug mode!");
			return std::string();
		}
	}

	bool DebugRenderer::OnKeyPressed(KeyPressedEvent& event)
	{
		if (event.IsRepeat())
			return false;

		switch (event.GetKeyCode())
		{
			case Key::F1:
				s_DebugRendererMode = DebugRendererMode::Normal;
			break;

			case Key::F2:
				s_DebugRendererMode = DebugRendererMode::Wireframe;
			break;

			case Key::F3:
				s_DebugRendererMode = DebugRendererMode::Point;
			break;

			default:
				break;
		}
	}

}
