#pragma once

#include "DME/Core/Base.h"
#include "DME/Scene/Scene.h"
#include "DME/Scene/Entity.h"
#include "DME/Renderer/Texture.h"

#include "DME/Core/Layer.h"
#include "DME/Events/KeyEvent.h"

namespace DME {

    class SceneHierarchyPanel : public Layer {
    public:
        SceneHierarchyPanel() = default;
        SceneHierarchyPanel(const Ref<Scene>& context);

		virtual void OnAttach() override;
		virtual void OnDetach() override;

        void SetContext(const Ref<Scene>& context);
        Ref<Scene> GetContext() { return m_Context; }

        void OnImGuiRender() override;
        void OnEvent(Event& event) override;
        void OnUpdate(TimeStep ts) override;

        Entity GetSelectedEntity() const { return m_SelectionContext; }
        void ClearSelectedContext() { m_SelectionContext = {}; }
        void SetSelectedEntity(Entity entity);

        bool SelectionContextIsNull() { return !m_SelectionContext; }

		void DeleteSelectedEntity();

    private:
        void DrawEntityNode(Entity entity);
        bool OnKeyPressed(KeyPressedEvent& event);
        bool OnMouseButtonPressed(MouseButtonPressedEvent& event);

    private:
        Ref<Scene> m_Context;
        Entity m_SelectionContext;

        Ref<Texture2D> m_PlusSmallButton;

        std::string m_Search;

        bool m_SceneHierarchyPanelHovered = false;
        bool m_SceneHierarchyPanelFocused = false;
    };

}
