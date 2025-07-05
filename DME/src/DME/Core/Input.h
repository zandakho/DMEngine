#pragma once

// --- DME/Core/ ---------------------------------------------//
#include "DME/Core/KeyCodes.h"								//
#include "DME/Core/MouseCodes.h"								//
#include "DME/Core/Base.h"									//
// -------------------------------------------------------------//


namespace DME
{
	class Input
	{
	public:
		static bool IsKeyPressed(KeyCode keycode);

		static bool IsMouseButtonPressed(MouseCode button);
		static std::pair<float, float> GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();

	};
}