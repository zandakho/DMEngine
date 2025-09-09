#pragma once

#include <dme.h>

#include "DME/Core/Base.h"
#include "DME/Core/Log.h"
#include "DME/Scene/Scene.h"
#include "DME/Scene/Entity.h"
#include "DME/Renderer/Texture.h"

namespace DME
{

	class SceneHierarchyPanel : Layer
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Ref<Scene>& context);

		void SetContext(const Ref<Scene>& context);

		Ref<Scene> GetContext() { return m_Context; };

		bool SelectionContextIsNull() { return !m_SelectionContext.HasComponent<IDComponent>() && !m_SelectionContext.HasComponent<TagComponent>() ? true : false; }

		void OnImGuiRender() override;

		Entity GetSelectedEntity() const { return m_SelectionContext; }
		void ClearSelectedContext() { m_SelectionContext = {}; }
		void SetSelectedEntity(Entity entity);

		unsigned long long GetTextureID(const Ref<Texture2D>& texture) const;

		void OnEvent(Event& event) override;
		void OnUpdate(TimeStep ts) override;

	private:

		bool OnKeyPressed(KeyPressedEvent& event);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& event);

		template<typename T>
		void DisplayAddComponentEntry(const std::string& entryName);

		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity entity);

	private:
		Ref<Scene> m_Context;
		Entity m_SelectionContext;


		Ref<Texture2D> m_SettingsButton;
		uint64_t m_SettingsButtonTextureID;
	};
}