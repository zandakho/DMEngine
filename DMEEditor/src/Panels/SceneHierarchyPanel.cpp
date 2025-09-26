#include "dmepch.h"
#include "SceneHierarchyPanel.h"
#include "DME/Scene/Components.h"

#include "DME/ImGui/ImGuiDMEEditor.hpp"

#include <ImGui/imgui.h>

namespace DME {

    extern const std::filesystem::path g_AssetPath;

    SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context) 
    {
        SetContext(context);
    }

	void SceneHierarchyPanel::OnAttach()
	{
		m_PlusSmallButton = Texture2D::Create("Resources/Icons/SceneHieararchy/SH_PlusSmallIcon_Img.png");
		m_DeleteButton = Texture2D::Create("Resources/Icons/SceneHieararchy/SH_DeleteIcon_Img.png");


	}

	void SceneHierarchyPanel::OnDetach()
	{
		m_PlusSmallButton.reset();
		m_DeleteButton.reset();
	}

	void SceneHierarchyPanel::SetContext(const Ref<Scene>& context) 
	{
        m_Context = context;
        m_SelectionContext = {};
    }

	

	void SceneHierarchyPanel::OnImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy", &m_SceneHierarchyRender, ImGuiWindowFlags_NoCollapse);

		m_SceneHierarchyWindowFocused = ImGui::IsWindowFocused();
		m_SceneHierarchyWindowHovered = ImGui::IsWindowHovered();
		m_SceneHierarchyWindowDocked = ImGui::IsWindowDocked();

		static char buffer[256]{};
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		if (ImGui::InputTextWithHint("##Search", "Search ...", buffer, sizeof(buffer)))
			m_Search = buffer;

		if (ImGui::IsMouseClicked(1) && m_SceneHierarchyWindowFocused)
			ImGui::OpenPopup("AddEntityPopup");

		if (ImGui::BeginPopup("AddEntityPopup"))
		{
			if (ImGuiDMEEditor::IconButtonWithText("ADD##CreateEntityButton", reinterpret_cast<ImTextureID*>(static_cast<uintptr_t>(m_PlusSmallButton->GetRendererID()))))
			{
				m_Context->CreateEntity("Empty Entity");
			}
			ImGui::EndPopup();
		}

		m_Context->m_Registry.view<entt::entity>().each([&](auto entityID) {
			Entity entity{ entityID, m_Context.get() };
			DrawEntityNode(entity);
			});

		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			m_SelectionContext = {};

		ImGui::End();
	}

	void SceneHierarchyPanel::CreateSceneIfEmpty(Ref<Scene> scene)
    {
		if (!m_Context) m_Context = scene;
    }


	void SceneHierarchyPanel::DeleteSelectedEntity()
	{
		if (!GetSelectedEntity()) return;

		GetContext()->DestroyEntity(GetSelectedEntity());
		ClearSelectedContext();
	}

    bool SceneHierarchyPanel::OnKeyPressed(const KeyPressedEvent& event)
	{
        if (event.IsRepeat())
            return false;

        return false;
    }

    bool SceneHierarchyPanel::OnMouseButtonPressed(const MouseButtonPressedEvent& event)
	{
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

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 2));
        ImGui::PushStyleColor(ImGuiCol_Header, (m_SelectionContext == entity) ? ImGui::GetColorU32(ImVec4(0.2f, 0.6f, 0.9f, 0.5f)) : ImGui::GetColorU32(ImGuiCol_Header));
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, (m_SelectionContext == entity) ? ImGui::GetColorU32(ImVec4(0.3f, 0.7f, 1.0f, 0.6f)) : ImGui::GetColorU32(ImGuiCol_HeaderHovered));
        bool opened = ImGui::TreeNodeEx(reinterpret_cast<const void*>(static_cast<uint64_t>(entity.GetUUID())), flags, tag.c_str());

        if (ImGui::IsItemClicked())
            m_SelectionContext = entity;

        bool entityDeleted = false;
        if (ImGui::BeginPopupContextItem()) {
            std::string name = std::format("{}", tag);
            if (ImGuiDMEEditor::IconButtonWithText(name.c_str(), reinterpret_cast<ImTextureID*>(static_cast<uint64_t>(m_DeleteButton->GetRendererID()))))
                entityDeleted = true;
            ImGui::EndPopup();
        }

        if (opened)
            ImGui::TreePop();
		ImGui::PopStyleColor(2);

        if (entityDeleted) {
            bool isSelected = (GetSelectedEntity() == entity);
            GetContext()->DestroyEntity(entity);
            if (isSelected)
                ClearSelectedContext();
        }

		ImGui::PopStyleVar();
    }
}
