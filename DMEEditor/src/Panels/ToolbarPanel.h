#pragma once
#include <dme.h>
#include "DME/Core/Base.h"
#include "DME/Renderer/Texture.h"

namespace DME {

	class ToolbarPanel
	{
	public:
		ToolbarPanel() = default;
		void SetIcons(const Ref<Texture2D>& cursor, const Ref<Texture2D>& move, const Ref<Texture2D>& rotate, const Ref<Texture2D>& scale, const Ref<Texture2D>& play);
		void SetGizmoType(int* gizmoTypePtr) { m_GizmoTypePtr = gizmoTypePtr; }

		void OnImGuiRender();

	private:
		Ref<Texture2D> m_IconCursor, m_IconMove, m_IconRotate, m_IconScale, m_IconPlay;
		int* m_GizmoTypePtr = nullptr;
	};

}
