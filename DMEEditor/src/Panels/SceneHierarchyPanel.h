#pragma once

#include "DME/Core/Base.h"
#include "DME/Core/Log.h"
#include "DME/Scene/Scene.h"
#include "DME/Scene/Entity.h"
#include "DME/Renderer/Texture.h"
#include "DME/ImGui/ImGuiDMEEditor.h"

namespace DME
{
	
	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Ref<Scene>& context);

		void SetContext(const Ref<Scene>& context);

		void OnImGuiRender();

		Entity GetSelectedEntity() const { return m_SelectionContext; }
		void SetSelectedEntity(Entity entity);

	private:

		template<typename T>
		void DisplayAddComponentEntry(const std::string& entryName);

		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity entity);

	private:
		Ref<Scene> m_Context;
		Entity m_SelectionContext;

		Ref<Texture2D> m_PlusButtonSmall;
	};
}