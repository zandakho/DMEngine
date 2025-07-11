#include "dmepch.h"

#include "SceneHierarchyPanel.h"

#include <ImGui/imgui.h>
#include <ImGui/imgui_internal.h>


#include "DME/Scene/Components.h"
#include <glm/gtc/type_ptr.hpp>

#include "DME/Renderer/Renderer2D.h"

#ifdef _MSVC_LANG
	#define _CRT_SECURE_NO_WARNINGS
#endif

namespace DME
{
	static void DrawVec3Control(const std::string& label, glm::vec3& values, float min = -FLT_MAX, float max = FLT_MAX, float columnWidth = 200.0f, ImGuiSliderFlags flags = ImGuiSliderFlags_ClampOnInput | ImGuiSliderFlags_WrapAround)
	{
		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth - 110.0f);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth() + 50);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 5));

		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[SceneHierarchyPanel::SetFont(OpenSansRegular_21)]);
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
			ImGui::PopStyleVar();

			if (removeComponent)
				entity.RemoveComponent<T>();

		}
	}

	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context)
	{
		SetContext(context);
	}

	void SceneHierarchyPanel::SetContext(const Ref<Scene>& context)
	{
		m_Context = context;
		m_SelectionContext = {};
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

		if (ImGui::BeginPopupContextWindow(0, 1 | ImGuiPopupFlags_NoOpenOverExistingPopup))
		{
			if (ImGui::MenuItem("Create Empty entity"))
				m_Context->CreateEntity("Empty entity");
			ImGui::EndPopup();
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

			m_Context->DestroyEntity(entity);
			if (m_SelectionContext == entity)
				m_SelectionContext = {};
		}
	}

	void SceneHierarchyPanel::DrawComponents(Entity entity)
	{
		auto& tag = entity.GetComponent<TagComponent>().Tag;

		if (entity.HasComponent<TagComponent>())
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
		ImGui::PushItemWidth(-1);

		ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionMax().x);
		if (ImGui::Button("+ADD"))
			ImGui::OpenPopup("DialogWindow");

		if (ImGui::BeginPopup("DialogWindow"))
		{
			if (!entity.HasComponent<CameraComponent>())
			{
				if (ImGui::MenuItem("Camera"))
				{
					m_SelectionContext.AddComponent<CameraComponent>();
					DME_CORE_INFO("A 'Camera' component has been added to the '{0}' entity", tag);
					ImGui::CloseCurrentPopup();
				}
			}
			

			if (!entity.HasComponent<SpriteRendererComponent>())
			{
				if (ImGui::MenuItem("Sprite Renderer"))
				{
					m_SelectionContext.AddComponent<SpriteRendererComponent>();
					DME_CORE_INFO("A 'Sprite Renderer' component has been added to the '{0}' entity", tag);
					ImGui::CloseCurrentPopup();

				}
			}
			

			if (!entity.HasComponent<TransformComponent>())
			{
				if (ImGui::MenuItem("Transform"))
				{
					m_SelectionContext.AddComponent<TransformComponent>();
					DME_CORE_INFO("A 'Transform' component has been added to the '{0}' entity", tag);
					ImGui::CloseCurrentPopup();

				}
			}
			

			ImGui::EndPopup();
		}

		ImGui::PopItemWidth();

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
		});
		
		
	}


}
