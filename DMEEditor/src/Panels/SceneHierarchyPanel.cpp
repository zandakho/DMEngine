#include "dmepch.h"
#include "SceneHierarchyPanel.h"
#include "DME/Scene/Components.h"

#include "DME/ImGui/ImGuiDMEEditor.h"

#include <ImGui/imgui.h>
#include <ImGui/imgui_internal.h>

namespace DME {

    extern const std::filesystem::path g_AssetPath;

    SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context) 
    {
        SetContext(context);
    }

	void SceneHierarchyPanel::OnAttach()
	{
	}

	void SceneHierarchyPanel::OnDetach()
	{

	}

	void SceneHierarchyPanel::SetContext(const Ref<Scene>& context) {
        m_Context = context;
        m_SelectionContext = {};
    }

    void SceneHierarchyPanel::OnImGuiRender() 
    {
		m_PlusSmallButton = Texture2D::Create("Resources/Icons/SceneHieararchy/Plus_Small_Green_Img.png");

        if (!m_Context)
            return;

        ImGui::Begin("Scene Hierarchy");

        static char buffer[256]{};
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        if (ImGui::InputTextWithHint("##Search", "Search ...", buffer, sizeof(buffer)))
            m_Search = buffer;

        m_Context->m_Registry.view<entt::entity>().each([&](auto entityID) {
            Entity entity{ entityID , m_Context.get() };
            DrawEntityNode(entity);
            });

        if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
            m_SelectionContext = {};

        if (ImGui::IsMouseClicked(1) && ImGui::IsWindowHovered())
            ImGui::OpenPopup("AddEntityPopup");
        if (ImGui::BeginPopup("AddEntityPopup"))
        {
            if (ImGuiDMEEditor::AddButton("ADD##CreateEntityButton", reinterpret_cast<ImTextureID*>(static_cast<uintptr_t>(m_PlusSmallButton->GetRendererID()))))
            {
				m_Context->CreateEntity("Empty Entity");
            }
			ImGui::EndPopup();
        }

        m_SceneHierarchyPanelFocused = ImGui::IsWindowFocused();
        m_SceneHierarchyPanelHovered = ImGui::IsWindowHovered();

        ImGui::End();
    }

    void SceneHierarchyPanel::SetSelectedEntity(Entity entity) {
        m_SelectionContext = entity;
    }

	void SceneHierarchyPanel::DeleteSelectedEntity()
	{
		GetContext()->DestroyEntity(GetSelectedEntity());
		ClearSelectedContext();
	}

	void SceneHierarchyPanel::OnEvent(Event& event) {
        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<KeyPressedEvent>(DME_BIND_EVENT_FN(SceneHierarchyPanel::OnKeyPressed));
        dispatcher.Dispatch<MouseButtonPressedEvent>(DME_BIND_EVENT_FN(SceneHierarchyPanel::OnMouseButtonPressed));
    }

    void SceneHierarchyPanel::OnUpdate(TimeStep ts) {}

    bool SceneHierarchyPanel::OnKeyPressed(KeyPressedEvent& event) {

        if (event.IsRepeat())
            return false;
        
        switch (event.GetKeyCode())
        {
        }

        return false;
    }

    bool SceneHierarchyPanel::OnMouseButtonPressed(MouseButtonPressedEvent& event) {
        return false;
    }

    void SceneHierarchyPanel::DrawEntityNode(Entity entity) {
        auto& tag = entity.GetComponent<TagComponent>().Tag;
        if (!m_Search.empty() && tag.find(m_Search) == std::string::npos)
            return;

        ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0)
            | ImGuiTreeNodeFlags_SpanAvailWidth
            | ImGuiTreeNodeFlags_OpenOnDoubleClick
            | ImGuiTreeNodeFlags_Framed;

        ImGui::PushStyleColor(ImGuiCol_Header, (m_SelectionContext == entity) ? ImGui::GetColorU32(ImVec4(0.2f, 0.6f, 0.9f, 0.5f)) : ImGui::GetColorU32(ImGuiCol_Header));
        bool opened = ImGui::TreeNodeEx(reinterpret_cast<const void*>(static_cast<uint64_t>(entity.GetUUID())), flags, tag.c_str());
        ImGui::PopStyleColor();

        if (ImGui::IsItemClicked())
            m_SelectionContext = entity;

        bool entityDeleted = false;
        if (ImGui::BeginPopupContextItem()) {
            std::string name = std::format("Delete {}", tag);
            if (ImGui::MenuItem(name.c_str()))
                entityDeleted = true;
            ImGui::EndPopup();
        }

        if (opened)
            ImGui::TreePop();

        if (entityDeleted) {
            bool isSelected = (GetSelectedEntity() == entity);
            GetContext()->DestroyEntity(entity);
            if (isSelected)
                ClearSelectedContext();
        }
    }

}
