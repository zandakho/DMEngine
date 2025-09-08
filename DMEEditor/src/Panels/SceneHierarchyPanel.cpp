#include "dmepch.h"

#include "SceneHierarchyPanel.h"
#include "DME/Scene/Components.h"

#include "DME/ImGui/ImGuiDMEEditor.h"
#include "DME/Renderer/Renderer2D.h"

#include "FontLibrary.h"

#include <ImGui/imgui.h>
#include <ImGui/imgui_internal.h>

#include <glm/gtc/type_ptr.hpp>



#ifdef _MSVC_LANG
	#define _CRT_SECURE_NO_WARNINGS
#endif

namespace DME
{
	extern const std::filesystem::path g_AssetPath;

	const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth;
	template<typename T, typename UFunction>
	static void DrawComponent(const std::string& name, Entity entity, UFunction uifunction)
	{ 
		std::string AddButton = std::format("+##{}", name.c_str());
		std::string DeleteButton = std::format("Delete##{}", name.c_str());
		std::string CancelButton = std::format("Cancel##{}", name.c_str());
		std::string DialogWindowName = std::format("Dialog {} window", name.c_str());
		if (entity.HasComponent<T>())
		{
			auto& component = entity.GetComponent<T>();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 4));
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(15, 10));
			if (ImGui::Button(AddButton.c_str()))
				ImGui::OpenPopup(DialogWindowName.c_str());
			bool removeComponent = false;
			if (ImGui::BeginPopup(DialogWindowName.c_str()))
			{
				if (ImGui::Button(DeleteButton.c_str()))
				{
					removeComponent = true;
					DME_CORE_INFO("The '{0}' component has been removed from the '{1}' object", name.c_str(), entity.GetComponent<TagComponent>().Tag);
				}
				ImGui::SameLine();
				if (ImGui::Button(CancelButton.c_str()))
					ImGui::CloseCurrentPopup();
				ImGui::EndPopup();
			}

			ImGui::SameLine();

			bool open = ImGui::TreeNodeEx(reinterpret_cast<const void*>(static_cast<size_t>(typeid(T).hash_code())), treeNodeFlags, name.c_str());
			if (open)
			{
				uifunction(component);
				ImGui::TreePop();
			}
			ImGui::PopStyleVar(2);

			if (removeComponent)
				entity.RemoveComponent<T>();

		}
	}

	

	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context)
	{
		SetContext(context);
		m_PlusButtonSmall = Texture2D::Create("Resources/Icons/SceneHieararchy/Plus_Small_Green_Img.png");
	}

	void SceneHierarchyPanel::SetContext(const Ref<Scene>& context)
	{
		m_Context = context;
		m_SelectionContext = {};
	}

	void SceneHierarchyPanel::OnImGuiRender()
	{

		ImGui::Begin("Scene Hierarchy");

		if (m_Context)
		{
			m_Context->m_Registry.view<entt::entity>().each([&](auto entityID)
			{
				Entity entity{ entityID , m_Context.get() };
				DrawEntityNode(entity);
			});

			if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
				m_SelectionContext = {};

		}

		if (ImGui::BeginPopupContextWindow(0, 1 | ImGuiPopupFlags_NoOpenOverExistingPopup))
		{
			if (ImGui::MenuItem("Create Empty Entity"))
				m_Context->CreateEntity("Empty Entity");

			ImGui::EndPopup();
		}

		ImGui::End();

		ImGui::Begin("Properties");

		if (m_SelectionContext)
		{
			auto& tag = m_SelectionContext.GetComponent<TagComponent>().Tag;
			std::string cast_name = std::format("{0}##{0}", tag);

			if (m_SelectionContext.HasComponent<TagComponent>())
			{

				char buffer[256];
				memset(buffer, 0, sizeof(buffer));
				strcpy_s(buffer, sizeof(buffer), tag.c_str());

				if (ImGui::InputText("##Label", buffer, sizeof(buffer)))
				{
					tag = std::string(buffer);
				}
			}

			ImGui::SameLine();

			if (ImGui::Button("+ADD"))
			{
				ImGui::OpenPopup("DialogWindow");
			}
			if (ImGui::BeginPopup("DialogWindow"))
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
			DrawComponents(m_SelectionContext);
		}

		ImGui::End();
	}

	void SceneHierarchyPanel::SetSelectedEntity(Entity entity)
	{
		m_SelectionContext = entity;
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		auto& tag = entity.GetComponent<TagComponent>().Tag;
		if (entity.HasComponent<TagComponent>())
		{
			tag = tag.empty() ? "Entity" : tag;
		}

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(9, 3));
		ImGuiTreeNodeFlags flags =  ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DrawLinesToNodes | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_Framed;
		bool opened = ImGui::TreeNodeEx(reinterpret_cast<const void*>(static_cast<uint64_t>(static_cast<uint32_t>(entity))), flags, tag.c_str());
		ImGui::PopStyleVar();
		if (ImGui::IsItemClicked())
		{
			m_SelectionContext = entity;
		}

		bool entityDeleted = false;
		if (ImGui::BeginPopupContextItem())
		{
			std::string name = std::format("Delete {}", tag);
			if (ImGui::MenuItem(name.c_str()))
				entityDeleted = true;

			ImGui::EndPopup();
		}

		if (opened)
		{

			ImGui::TreePop();

		}

		if (entityDeleted)
		{
			GetContext()->DestroyEntity(entity);
			if (GetSelectedEntity() == entity)
				ClearSelectedContext();
		}
	}

	

	static void DrawVec3Control(const std::string& label, glm::vec3& values, float min = -FLT_MAX, float max = FLT_MAX, float columnWidth = 200.0f, ImGuiSliderFlags flags = ImGuiSliderFlags_ClampOnInput | ImGuiSliderFlags_WrapAround)
	{
		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth - 110.0f);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth() + 50);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 5));

		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[static_cast<int>(FontLibrary::OpenSansBold_21)]);
		ImGui::DragFloat("##X", &values.x, 0.05f, min, max, "%.3f", flags | ImGuiSliderFlags_AsVectorX);
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::DragFloat("##Y", &values.y, 0.05f, min, max, "%.3f", flags | ImGuiSliderFlags_AsVectorY);
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::DragFloat("##Z", &values.z, 0.05f, min, max, "%.3f", flags | ImGuiSliderFlags_AsVectorZ);
		ImGui::PopFont();
		ImGui::PopStyleVar();
		ImGui::PopItemWidth();

		ImGui::Columns(1);
		ImGui::PopID();

	}

	void SceneHierarchyPanel::DrawComponents(Entity entity)
	{
		
		DrawComponent<TransformComponent>("Transform", entity, [](auto& component)
		{
			DrawVec3Control("Position", component.Position, -FLT_MAX, FLT_MAX);
			glm::vec3 rotation = glm::degrees(component.Rotation);
			DrawVec3Control("Rotation", rotation, -180, 180);
			component.Rotation = glm::radians(rotation);
			DrawVec3Control("Scale", component.Scale, -FLT_MAX, FLT_MAX);
		});

		DrawComponent<CameraComponent>("Camera", entity, [](auto& component)
		{
			auto& camera = component.Camera;

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(3, 3));
			ImGui::Checkbox("Primary", &component.Primary);
			ImGui::Checkbox("Fixed Aspect Ratio", &component.FixedAspectRatio);
			ImGui::PopStyleVar();

			const char* projectionTypeString[] = { "Perspective", "Orthographic" };
			const char* currentProjectionTypeString = projectionTypeString[static_cast<int>(camera.GetProjectionType())];

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
		});

		DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [](auto& component)
		{
			ImGui::ColorEdit4("Color", glm::value_ptr(component.Color), ImGuiColorEditFlags_NoInputs);

			ImGui::Button("##Texture", ImVec2(50.0f, 50.0f));
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					const wchar_t* path = (const wchar_t*)payload->Data;
					std::filesystem::path texturePath = std::filesystem::path(g_AssetPath) / path;
					Ref<Texture2D> texture = Texture2D::Create(texturePath.string());
					if (texture->IsLoaded())
						component.Texture = texture;
					else
						DME_WARNING("Could not load texture {0}", texturePath.filename().string());
				}
				ImGui::EndDragDropTarget();
			}
			ImGui::SameLine();
			ImGui::Text("Texture");
			ImGui::SameLine();
			if (ImGui::Button("N##Texture"))
				ImGui::OpenPopup("##TextureWindow");
			if (ImGui::BeginPopup("##TextureWindow"))
			{
				ImGui::DragFloat("Tiling Factor", &component.TilingFactor, 0.1f, 0.0f, 100.0f);

				ImGui::EndPopup();
			}
			

		});

		DrawComponent<CircleRendererComponent>("Circle Renderer", entity, [](auto& component)
			{
				ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
				ImGui::DragFloat("Thickness", &component.Thickness, 0.025f, 0.0f, 1.0f);
				ImGui::DragFloat("Fade", &component.Fade, 0.00025f, 0.0f, 1.0f);
			});

		DrawComponent<Rigidbody2DComponent>("Rigidbody 2D", entity, [](auto& component)
		{
			const char* bodyTypeStrings[] = { "Static", "Dynamic", "Kinematic" };
			const char* currentBodyTypeString = bodyTypeStrings[(int)component.Type];
			if (ImGui::BeginCombo("Body Type", currentBodyTypeString))
			{
				for (int i = 0; i < 2; i++)
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
		});

		DrawComponent<BoxCollider2DComponent>("Box Collider 2D", entity, [](auto& component)
		{
			ImGui::DragFloat2("Offset", glm::value_ptr(component.Offset));
			ImGui::DragFloat2("Size", glm::value_ptr(component.Size));
			ImGui::DragFloat("Density", &component.Density, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Restitution Threshold", &component.RestitutionThreshold, 0.01f, 0.0f);
		});

		DrawComponent<CircleCollider2DComponent>("Circle Collider 2D", entity, [](auto& component)
		{
			ImGui::DragFloat2("Offset", glm::value_ptr(component.Offset));
			ImGui::DragFloat("Radius", &component.Radius);
			ImGui::DragFloat("Density", &component.Density, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Restitution Threshold", &component.RestitutionThreshold, 0.01f, 0.0f);
		});
		
	}

	template<typename T>
	void SceneHierarchyPanel::DisplayAddComponentEntry(const std::string& entryName) {
		if (!m_SelectionContext.HasComponent<T>())
		{
			if (ImGui::MenuItem(entryName.c_str()))
			{
				m_SelectionContext.AddComponent<T>();
				ImGui::CloseCurrentPopup();
			}
		}
	}


}
