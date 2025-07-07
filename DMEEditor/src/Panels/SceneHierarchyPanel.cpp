#include "dmepch.h"


#include "SceneHierarchyPanel.h"

#include <ImGui/imgui.h>
#include <ImGui/imgui_internal.h>

#include "DME/Scene/Components.h"
#include <glm/gtc/type_ptr.hpp>
#include <ImGui/backends/imgui_impl_opengl3_loader.h>

#include "DME/Renderer/Renderer2D.h"
#include "ImGui/imgui_internal.h"

namespace DME
{
	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context)
	{
		SetContext(context);
	}

	void SceneHierarchyPanel::SetContext(const Ref<Scene>& context)
	{
		m_Context = context;
	}

	void SceneHierarchyPanel::OnImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy");

		m_Context->m_Registry.view<entt::entity>().each([&](auto entityID)
		{
			Entity entity = { entityID, m_Context.get() };
			DrawEntityNode(entity);

		});

		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
		{
			m_SelectionContext = {};
		}

		ImGui::End();

		
		ImGuiIO& io = ImGui::GetIO(); static_cast<void>(io);

		ImGui::Begin("Properties", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_HorizontalScrollbar);

		if (m_SelectionContext)
		{
			DrawComponents(m_SelectionContext);
		}

		ImGui::End();

		ImGui::Begin("Renderer Stats");

		ImGui::Text("Draw Calls: %d", Renderer2D::GetStats().DrawCalls);
		ImGui::Text("Quads: %d", Renderer2D::GetStats().QuadCount);
		ImGui::Text("Vertices: %d", Renderer2D::GetStats().GetTotalVertexCount());
		ImGui::Text("Indices: %d", Renderer2D::GetStats().GetTotalIndexCount());
		ImGui::Text("FPS: %d", static_cast<uint16_t>(io.Framerate));
		ImGui::Text("Frame time: %.3f ms", static_cast<uint16_t>(io.Framerate) / 1000.0f);

		ImGui::End();

	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{

		auto& tag = entity.GetComponent<TagComponent>().Tag;
		if (entity.HasComponent<TagComponent>())
		{
			tag = tag.empty() ? "Entity" : tag;
		}

		ImGuiTreeNodeFlags flags =  ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		bool opened = ImGui::TreeNodeEx(reinterpret_cast<const void*>(static_cast<uint64_t>(static_cast<uint32_t>(entity))), flags, tag.c_str());
		if (ImGui::IsItemClicked())
		{
			m_SelectionContext = entity;
		}

		if (opened)
		{

			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
			bool opened = ImGui::TreeNodeEx(reinterpret_cast<const void*>(static_cast<uint64_t>(static_cast<uint32_t>(entity)) + 1000), flags, tag.c_str());
			if (opened)
				ImGui::TreePop(); 
			ImGui::TreePop();
		}
	}

	static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 200.0f)
	{
		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth() + 50);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2, 2));

		ImGui::DragFloat("##X", &values.x, 0.1f, -FLT_MAX, FLT_MAX, "%.3f", ImGuiSliderFlags_AsVectorX);
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::DragFloat("##Y", &values.y, 0.1f, -FLT_MAX, FLT_MAX, "%.3f", ImGuiSliderFlags_AsVectorY);
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::DragFloat("##Z", &values.z, 0.1f, -FLT_MAX, FLT_MAX, "%.3f", ImGuiSliderFlags_AsVectorZ);
		ImGui::PopStyleVar();
		ImGui::PopItemWidth();

		ImGui::Columns(1);
		ImGui::PopID();

		
	}

	void SceneHierarchyPanel::DrawComponents(Entity entity)
	{
		if (entity.HasComponent<TagComponent>())
		{
			auto& tag = entity.GetComponent<TagComponent>().Tag;

			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, sizeof(buffer), tag.c_str());

			if (ImGui::InputText("Label", buffer, sizeof(buffer)))
			{
				tag = std::string(buffer);
			}
		}

		if (entity.HasComponent<TransformComponent>())
		{
			if (ImGui::TreeNodeEx(reinterpret_cast<const void*>(static_cast<size_t>(typeid(TransformComponent).hash_code())), ImGuiTreeNodeFlags_DefaultOpen, "Transform"))
			{

				auto& transform = entity.GetComponent<TransformComponent>();

				DrawVec3Control("Position", transform.Position);
				DrawVec3Control("Rotation", transform.Rotation);
				DrawVec3Control("Scale", transform.Scale, 1.0f);

				ImGui::TreePop();


			}
		}

		if (entity.HasComponent<CameraComponent>())
		{
			if (ImGui::TreeNodeEx(reinterpret_cast<const void*>(static_cast<size_t>(typeid(CameraComponent).hash_code())), ImGuiTreeNodeFlags_DefaultOpen, "Camera"))
			{
				auto& cameraComponent = entity.GetComponent<CameraComponent>();
				auto& camera = cameraComponent.Camera;

				ImGui::Checkbox("Primary", &cameraComponent.Primary);

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

					if (ImGui::Checkbox("Fixed Aspect Ratio", &cameraComponent.FixedAspectRatio));

				}


				ImGui::TreePop();
			}
		}

		if (entity.HasComponent<SpriteRendererComponent>())
		{

			if (ImGui::TreeNodeEx(reinterpret_cast<const void*>(static_cast<size_t>(typeid(SpriteRendererComponent).hash_code())), ImGuiTreeNodeFlags_DefaultOpen, "Sprite Renderer"))
			{
				auto& sprite = entity.GetComponent<SpriteRendererComponent>();
				ImGui::ColorEdit4("Color", glm::value_ptr(sprite.Color), ImGuiColorEditFlags_AlphaPreviewHalf);

				ImGui::TreePop();
			}

		}
	}





}
