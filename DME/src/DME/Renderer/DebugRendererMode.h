#pragma once

namespace DME
{
	enum class DebugRendererMode
	{
		Normal = 0,
		Wireframe = 1,
		Point = 2
	};


	extern std::string DebugModeToString(DebugRendererMode mode);

	extern DebugRendererMode s_DebugRendererMode;
}