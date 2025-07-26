#pragma once

#include "DME/Core/Base.h"
#include "DME/Core/Log.h"
#include "DME/Scene/Scene.h"
#include "DME/Scene/Entity.h"

#include "FontLibrary.h"

namespace DME
{
	
	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Ref<Scene>& context);

		void SetContext(const Ref<Scene>& context);

		void OnImGuiRender();
		static int SetFont(FontLibrary font) { return static_cast<int>(font); }

		Entity GetSelectedEntity() const { return m_SelectionContext; }
		void SetSelectedEntity(Entity entity);

	private:
		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity entity);

	private:
		Ref<Scene> m_Context;
		Entity m_SelectionContext;

	};
}