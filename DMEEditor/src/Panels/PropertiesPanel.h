#pragma once

#include "DME/Core/Base.h"
#include "DME/Events/Event.h"
#include "DME/Events/KeyEvent.h"
#include "DME/Events/MouseEvent.h"
#include "DME/Scene/Scene.h"
#include "DME/Scene/Entity.h"
#include "DME/Core/Layer.h"
#include "DME/Renderer/Texture.h"

namespace DME {

    class PropertiesPanel : public Layer
    {
	public: // Constructors and Destructors
        PropertiesPanel() = default;

	public: // Helpers (Get&Set)
		
		void SetContext(Entity entity) { m_SelectedEntity = entity; }
		Entity GetSelectedEntity() const { return m_SelectedEntity; }

		bool GetFulltexturePack() const;
		void ClearTexturePack();

	public: // Layer overrides
		void OnAttach() override;
		void OnDetach() override;
        void OnImGuiRender() override;

	public: // Events

    	static bool OnKeyPressed(const KeyPressedEvent& event);
		static bool OnMouseButtonPressed(const MouseButtonPressedEvent& event);

    public:

		bool m_PropertiesPanelRender;

    private: // Templates UI

        template<typename T, typename UFunction>
        void DrawComponent(const std::string& name, Entity entity, UFunction uifunction);

        template<typename T>
        void DisplayAddComponentEntry(const std::string& entryName);

	private: // UI

		void DrawVec3Control(const std::string& label, glm::vec3& values);

    private:

        Entity m_SelectedEntity;

	private: // Textures

        Ref<Texture2D> m_PlusSmallButtonIcon;
        Ref<Texture2D> m_DeleteButtonIcon;
        Ref<Texture2D> m_SettingsButtonIcon;
        Ref<Texture2D> m_ResetButtonIcon;
    };

}
