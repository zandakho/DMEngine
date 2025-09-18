#pragma once

#include "DME/Core/Base.h"
#include "DME/Scene/Scene.h"
#include "DME/Scene/Entity.h"
#include "DME/Renderer/Texture.h"

#include "DME/Core/Layer.h"
#include "DME/Events/KeyEvent.h"

namespace DME {

	class SceneHierarchyPanel : public Layer 
	{
	public: // Constructors and Destructors

		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Ref<Scene>& context);

	public: // Helpers (Get&Set)

		void SetContext(const Ref<Scene>& context);
		Ref<Scene> GetContext() { return m_Context; }

		Entity GetSelectedEntity() const { return m_SelectionContext; }
		void ClearSelectedContext() { m_SelectionContext = {}; }
		void SetSelectedEntity(Entity entity) { m_SelectionContext = entity; }

		void DeleteSelectedEntity();

		bool GetTextureFullPack() const;
		void ClearTexturePack();

	public: // Layer overrides

		void OnAttach() override;
		void OnDetach() override;
		void OnImGuiRender() override;

	public: // Events

		bool OnKeyPressed(const KeyPressedEvent& event);
		bool OnMouseButtonPressed(const MouseButtonPressedEvent& event);

	public: // UI

		void DrawEntityNode(Entity entity);

	private: // Context

		Ref<Scene> m_Context;
		Entity m_SelectionContext;

	private: // Textures
		Ref<Texture2D> m_PlusSmallButton;
		Ref<Texture2D> m_DeleteButton;

		std::string m_Search;

	private: // For debug

		bool m_SceneHierarchyWindowFocused;
		bool m_SceneHierarchyWindowHovered;
		bool m_SceneHierarchyWindowDocked;

	};

}
