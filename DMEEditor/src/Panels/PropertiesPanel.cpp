#include "dmepch.h"

#include "PropertiesPanel.h"
#include "DME/Scene/Components.h"


#include "DME/ImGui/ImGuiDMEEditor.hpp"

#include <glm/gtc/type_ptr.hpp>

namespace DME {

    extern const std::filesystem::path g_AssetPath;
    const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth;

	void PropertiesPanel::OnAttach()
	{
		m_PlusSmallButtonIcon = Texture2D::Create("Resources/Icons/Properties/Properties_PlusSmallIcon_Img.png");
		m_DeleteButtonIcon = Texture2D::Create("Resources/Icons/Properties/Properties_DeleteIcon_Img.png");
		m_SettingsButtonIcon = Texture2D::Create("Resources/Icons/Properties/Properties_SettingsIcon_Img.png");
		m_ResetButtonIcon = Texture2D::Create("Resources/Icons/Properties/Properties_ResetIcon_Img.png");
	}

	void PropertiesPanel::OnDetach()
	{
		m_PlusSmallButtonIcon.reset();
		m_DeleteButtonIcon.reset();
		m_SettingsButtonIcon.reset();
		m_ResetButtonIcon.reset();
	}

	void PropertiesPanel::OnImGuiRender()
	{

        ImGui::Begin("Properties", &m_PropertiesPanelRender, ImGuiWindowFlags_NoCollapse);

        if (m_SelectedEntity) 
		{
            auto& tag = m_SelectedEntity.GetComponent<TagComponent>().Tag;

            char buffer[256];
            memset(buffer, 0, sizeof(buffer));
            strcpy_s(buffer, sizeof(buffer), tag.c_str());
            if (ImGui::InputText("##Label", buffer, sizeof(buffer)))
                tag = std::string(buffer);

            ImGui::SameLine();
            if (ImGuiDMEEditor::IconButtonWithText("ADD##PropertiesPanelButton", reinterpret_cast<ImTextureID*>(static_cast<uint64_t>(m_PlusSmallButtonIcon->GetRendererID()))))
                ImGui::OpenPopup("AddComponentPopup");

            if (ImGui::BeginPopup("AddComponentPopup")) 
			{
                DisplayAddComponentEntry<CameraComponent>("Camera");
                DisplayAddComponentEntry<SpriteRendererComponent>("Sprite Renderer");
                DisplayAddComponentEntry<CircleRendererComponent>("Circle Renderer");
                DisplayAddComponentEntry<Rigidbody2DComponent>("Rigidbody 2D");
                DisplayAddComponentEntry<BoxCollider2DComponent>("Box Collider 2D");
                DisplayAddComponentEntry<CircleCollider2DComponent>("Circle Collider 2D");
                ImGui::EndPopup();
            }

            ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 3);

            DrawComponent<TransformComponent>("Transform", m_SelectedEntity, [this](auto& component) 
				{
                DrawVec3Control("Position", component.Position);
                glm::vec3 rotation = glm::degrees(component.Rotation);
                DrawVec3Control("Rotation", rotation);
                component.Rotation = glm::radians(rotation);
                DrawVec3Control("Scale", component.Scale);
                });

			DrawComponent<CameraComponent>("Camera", m_SelectedEntity, [](auto& component)
				{
					auto& camera = component.Camera;

					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(3, 3));
					ImGui::Checkbox("Primary", &component.Primary);
					ImGui::Checkbox("Fixed Aspect Ratio", &component.FixedAspectRatio);
					ImGui::PopStyleVar();

					const char* projectionTypeString[] = { "Perspective", "Orthographic" };
					const char* currentProjectionTypeString = projectionTypeString[static_cast<int>(camera.GetProjectionType())];

					ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - 100);
					if (ImGui::BeginCombo("Projection", currentProjectionTypeString))
					{
						for (int i = 0; i < 2; i++)
						{
							bool isSelected = currentProjectionTypeString == projectionTypeString[i];
							if (ImGui::Selectable(projectionTypeString[i], isSelected))
							{
								currentProjectionTypeString = projectionTypeString[i];
								camera.SetProjectionType(static_cast<SceneCamera::ProjectionType>(i));
							}
							if (isSelected)
								ImGui::SetItemDefaultFocus();
						}

						ImGui::EndCombo();
					}

					if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
					{
						float perspectiveVerticalFOV = glm::degrees(camera.GetPerspectiveVerticalFOV());
						if (ImGui::DragFloat("Vertical FOV", &perspectiveVerticalFOV))
							camera.SetPerspectiveVerticalFOV(glm::radians(perspectiveVerticalFOV));

						float perspectiveNear = camera.GetPerspectiveNearClip();
						if (ImGui::DragFloat("Near", &perspectiveNear))
							camera.SetPerspectiveNearClip(perspectiveNear);

						float perspectiveFar = camera.GetPerspectiveFarClip();
						if (ImGui::DragFloat("Far", &perspectiveFar))
							camera.SetPerspectiveFarClip(perspectiveFar);
					}

					if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
					{
						float orthographicSize = camera.GetOrthographicSize();
						if (ImGui::DragFloat("Size", &orthographicSize))
							camera.SetOrthographicSize(orthographicSize);

						float orthographicNear = camera.GetOrthographicNearClip();
						if (ImGui::DragFloat("Near", &orthographicNear))
							camera.SetOrthographicNearClip(orthographicNear);

						float orthographicFar = camera.GetOrthographicFarClip();
						if (ImGui::DragFloat("Far", &orthographicFar))
							camera.SetOrthographicFarClip(orthographicFar);
					}
					ImGui::PopItemWidth();
				});

			DrawComponent<SpriteRendererComponent>("Sprite Renderer", m_SelectedEntity, [this](auto& component)
				{
					ImGui::Text("Color");
					ImGui::SameLine();
					ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical, 3.0f);
					ImGui::SameLine();
					ImGui::ColorEdit4("##Color", glm::value_ptr(component.Color), ImGuiColorEditFlags_NoInputs);

					ImGui::Separator();

					ImGui::Text("Texture");
					ImGui::SameLine();

					if (component.Texture)
					{
						ImGuiDMEEditor::IconButton("##Texture",
							reinterpret_cast<ImTextureID*>(static_cast<uint64_t>(component.Texture->GetRendererID())),
							{ 50.0f, 50.0f });
					}
					else
					{
						ImGuiDMEEditor::IconButton("##Texture", nullptr, { 50.0f, 50.0f });
					}

					if (ImGui::BeginDragDropTarget())
					{
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
						{
							const wchar_t* path = static_cast<const wchar_t*>(payload->Data);
							std::filesystem::path texturePath = std::filesystem::path(g_AssetPath) / path;
							Ref<Texture2D> texture = Texture2D::Create(texturePath.string());
							if (texture->IsLoaded())
							{
								component.Texture = texture;
							}
							else
								DME_WARNING("Could not load texture {0}", texturePath.filename().string())
						}
						ImGui::EndDragDropTarget();
					}
					ImGui::SameLine();
					ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical, 3.0f);
					ImGui::SameLine();
					if (ImGuiDMEEditor::IconButton("##TextureSettings", reinterpret_cast<ImTextureID*>(static_cast<uint64_t>(m_SettingsButtonIcon->GetRendererID())), {30.0f, 30.0f}))
						ImGui::OpenPopup("##TextureWindow");
					if (ImGui::BeginPopup("##TextureWindow"))
					{
						if (ImGuiDMEEditor::IconButton("##RemoveTexture", reinterpret_cast<ImTextureID*>(static_cast<uint64_t>(m_DeleteButtonIcon->GetRendererID())), { 30.0f, 30.0f }))
						{
							if (component.Texture)
							{
								component.Texture = nullptr;
							}
						}
						ImGui::SameLine();
						ImGui::DragFloat("Tiling Factor", &component.TilingFactor, 0.1f, 0.0f, 100.0f);

						ImGui::EndPopup();
					}
				});

			DrawComponent<CircleRendererComponent>("Circle Renderer", m_SelectedEntity, [](auto& component)
				{
					ImGui::Text("Color");
					ImGui::SameLine();
					ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical, 3.0f);
					ImGui::SameLine();
					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 100);
					ImGui::ColorEdit4("##Color", glm::value_ptr(component.Color), ImGuiColorEditFlags_NoInputs);
					ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - 100);
					ImGui::DragFloat("Thickness", &component.Thickness, 0.025f, 0.0f, 1.0f);
					ImGui::DragFloat("Fade", &component.Fade, 0.00025f, 0.0f, 1.0f);
					ImGui::PopItemWidth();
				});

			DrawComponent<Rigidbody2DComponent>("Rigidbody 2D", m_SelectedEntity, [](auto& component)
				{
					const char* bodyTypeStrings[] = { "Static", "Dynamic", "Kinematic" };
					const char* currentBodyTypeString = bodyTypeStrings[(int)component.Type];

					ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - 100);
					if (ImGui::BeginCombo("Body Type", currentBodyTypeString))
					{
						for (int i = 0; i < 3; i++)
						{
							bool isSelected = currentBodyTypeString == bodyTypeStrings[i];
							if (ImGui::Selectable(bodyTypeStrings[i], isSelected))
							{
								currentBodyTypeString = bodyTypeStrings[i];
								component.Type = (Rigidbody2DComponent::BodyType)i;
							}

							if (isSelected)
								ImGui::SetItemDefaultFocus();
						}

						ImGui::EndCombo();
					}

					ImGui::Checkbox("Fixed Rotation", &component.FixedRotation);
					ImGui::PopItemWidth();
				});

			DrawComponent<BoxCollider2DComponent>("Box Collider 2D", m_SelectedEntity, [](auto& component)
				{
					ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - 100);

					ImGui::DragFloat2("Offset", glm::value_ptr(component.Offset));
					ImGui::DragFloat2("Size", glm::value_ptr(component.Size));
					ImGui::DragFloat("Density", &component.Density, 0.01f, 0.0f, 1.0f);
					ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f);
					ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 1.0f);
					ImGui::DragFloat("Restitution Threshold", &component.RestitutionThreshold, 0.01f, 0.0f);

					ImGui::PopItemWidth();
				});

			DrawComponent<CircleCollider2DComponent>("Circle Collider 2D", m_SelectedEntity, [](auto& component)
				{
					ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - 100);

					ImGui::DragFloat2("Offset", glm::value_ptr(component.Offset));
					ImGui::DragFloat("Radius", &component.Radius);
					ImGui::DragFloat("Density", &component.Density, 0.01f, 0.0f, 1.0f);
					ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f);
					ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 1.0f);
					ImGui::DragFloat("Restitution Threshold", &component.RestitutionThreshold, 0.01f, 0.0f);

					ImGui::PopItemWidth();
				});
        }

        ImGui::End();
    }

    template<typename T, typename UFunction>
    void PropertiesPanel::DrawComponent(const std::string& name, Entity entity, UFunction uifunction) 
	{
		std::string SettingsButton = std::format("##{0}", name.c_str());
		std::string DeleteButton = std::format("##{0}", name.c_str());
		std::string DialogWindow = std::format("##{0}", name.c_str());

        if (entity.HasComponent<T>()) {
            auto& component = entity.GetComponent<T>();
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 4));
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(15, 10));

			bool removeComponent = false;

			if (ImGuiDMEEditor::IconButton(SettingsButton.c_str(), reinterpret_cast<ImTextureID*>(static_cast<uint64_t>(m_SettingsButtonIcon->GetRendererID())), { 25.0f, 25.0f }))
				ImGui::OpenPopup(DialogWindow.c_str());
			if (ImGui::BeginPopup(DialogWindow.c_str()))
			{
				if (ImGuiDMEEditor::IconButton(DeleteButton.c_str(), reinterpret_cast<ImTextureID*>(static_cast<uint64_t>(m_DeleteButtonIcon->GetRendererID())), {25.0f, 25.0f}))
					removeComponent = true;
				ImGui::EndPopup();
			}

			ImGui::SameLine();
            bool open = ImGui::TreeNodeEx(reinterpret_cast<const void*>(typeid(T).hash_code()), treeNodeFlags, name.c_str());
            if (open) {
                uifunction(component);
                ImGui::TreePop();
            }
            ImGui::PopStyleVar(2);

            if (removeComponent)
                entity.RemoveComponent<T>();
        }
    }

	bool PropertiesPanel::OnKeyPressed(const KeyPressedEvent& event) 
	{

		if (event.IsRepeat())
			return false;

		return false;
	}

	bool PropertiesPanel::OnMouseButtonPressed(const MouseButtonPressedEvent& event) 
	{
		return false;
	}

    template<typename T>
    void PropertiesPanel::DisplayAddComponentEntry(const std::string& entryName) 
	{
        if (!m_SelectedEntity.HasComponent<T>()) {
            if (ImGui::MenuItem(entryName.c_str())) {
                m_SelectedEntity.AddComponent<T>();
                ImGui::CloseCurrentPopup();
            }
        }
    }

    void PropertiesPanel::DrawVec3Control(const std::string& label, glm::vec3& values)
	{
        ImGui::PushID(label.c_str());
        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, 90.0f);
        ImGui::Text(label.c_str());
        ImGui::NextColumn();
		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth() + 50);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 5));
        ImGui::DragFloat("##ValuesX", &values.x, 0.1f, -FLT_MAX, FLT_MAX, "%.2f", ImGuiSliderFlags_DrawBorderX_);
		ImGui::SameLine();
		ImGui::DragFloat("##ValuesY", &values.y, 0.1f, -FLT_MAX, FLT_MAX, "%.2f", ImGuiSliderFlags_DrawBorderY_);
		ImGui::SameLine();
		ImGui::DragFloat("##ValuesZ", &values.z, 0.1f, -FLT_MAX, FLT_MAX, "%.2f", ImGuiSliderFlags_DrawBorderZ_);
		ImGui::SameLine();
		if (ImGuiDMEEditor::IconButton("##TextureSettings", reinterpret_cast<ImTextureID*>(static_cast<uint64_t>(m_ResetButtonIcon->GetRendererID())), {25.0f, 25.0f}))
		{
			values.x = 0.0f;
			values.y = 0.0f;
			values.z = 0.0f;
		}
		ImGui::PopStyleVar();
		ImGui::PopItemWidth();
        ImGui::Columns(1);
        ImGui::PopID();
    }

}
