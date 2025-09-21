#pragma once

#include "DME/Events/KeyEvent.h"

#include "DME/Core/Layer.h"

namespace DME
{
	enum class DebugRendererMode
	{
		Normal = 0,
		Wireframe = 1,
		Point = 2
	};

	class DebugRenderer : public Layer
	{
	public: // Constructors and Destructors

		DebugRenderer();

	public: // Helper functions

		std::string DebugModeToString(DebugRendererMode mode);

		bool OnKeyPressed(const KeyPressedEvent& event);
	};

	extern DebugRendererMode s_DebugRendererMode;

}
