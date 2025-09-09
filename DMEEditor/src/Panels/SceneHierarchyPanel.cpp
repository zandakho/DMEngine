#include "dmepch.h"
#include "SceneHierarchyPanel.h"
#include "DME/Scene/Components.h"

#include <ImGui/imgui.h>
#include <ImGui/imgui_internal.h>

namespace DME {

    extern const std::filesystem::path g_AssetPath;

    SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context) {
        SetContext(context);
        m_PlusSmallButton = Texture2D::Create("Resource/Icons/SceneHierarchy/Plus_Small_Green_Img.png");
    }

    void SceneHierarchyPanel::SetContext(const Ref<Scene>& context) {
        m_Context = context;
        m_SelectionContext = {};
    }

    void SceneHierarchyPanel::OnImGuiRender() {
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

        if (ImGui::BeginPopupContextWindow(0, 1 | ImGuiPopupFlags_NoOpenOverExistingPopup)) {
            if (ImGui::MenuItem("Create Empty Entity"))
                m_Context->CreateEntity("Empty Entity");
            ImGui::EndPopup();
        }

        ImGui::End();
    }

    void SceneHierarchyPanel::SetSelectedEntity(Entity entity) {
        m_SelectionContext = entity;
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

        bool opened = ImGui::TreeNodeEx(reinterpret_cast<const void*>(static_cast<uint64_t>(entity.GetUUID())), flags, tag.c_str());

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
