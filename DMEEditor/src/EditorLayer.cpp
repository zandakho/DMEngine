#include "dmepch.h"

#include "EditorLayer.h"
#include "DME/Scene/SceneSerializer.h"

#include <ImGui/imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include "DME/Utils/PlatformUtils.h"

#include "ImGuizmo.h"
#include "DME/Math/Math.h" 

namespace DME
{

	EditorLayer::EditorLayer() : Layer("EditorLayer"), m_CameraController(1280.0f / 720.0f) { }


	void EditorLayer::OnAttach()
	{
		DME_PROFILE_FUNCTION(); 

		FramebufferSpecification fbSpec;
		fbSpec.Width = 1280;
		fbSpec.Width = 720;
		m_Framebuffer = Framebuffer::Create(fbSpec);

		m_ActiveScene = CreateRef<Scene>();

		m_EditorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);

#if 0
		class CameraController : public ScriptableEntity
		{
		public:

			void OnCreate()
			{
				auto& position = GetComponent<TransformComponent>().Position;
				position.x = rand() % 10 - 5.0f;
			}

			void OnDestroy()
			{

			}

			void OnUpdate(TimeStep ts)
			{

				auto& position = GetComponent<TransformComponent>().Position;
				float speed = 5.0f;

				if (Input::IsKeyPressed(Key::LeftControl) && Input::IsKeyPressed(Key::Left))
					position.x -= speed * ts;
				if (Input::IsKeyPressed(Key::LeftControl) && Input::IsKeyPressed(Key::Right))
					position.x += speed * ts;
				if (Input::IsKeyPressed(Key::LeftControl) && Input::IsKeyPressed(Key::Down))
					position.y -= speed * ts;
				if (Input::IsKeyPressed(Key::LeftControl) && Input::IsKeyPressed(Key::Up))
					position.y += speed * ts;
			}
		};
#endif
		
		m_SceneHierarchy.SetContext(m_ActiveScene);

	}

	void EditorLayer::OnDetach()
	{
		DME_PROFILE_FUNCTION();

	}

	void EditorLayer::OnUpdate(TimeStep ts)
	{
		DME_PROFILE_FUNCTION();

		if (FramebufferSpecification spec = m_Framebuffer->GetSpecification(); 
			m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && 
			(spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
		{
			m_Framebuffer->Resize(static_cast<uint32_t>(m_ViewportSize.x), static_cast<uint32_t>(m_ViewportSize.y));
			m_CameraController.OnResize(m_ViewportSize.x, m_ViewportSize.y);
			m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
			m_ActiveScene->OnViewportResize(static_cast<uint32_t>(m_ViewportSize.x), static_cast<uint32_t>(m_ViewportSize.y));

		}
		
		if (m_ViewportFocused)
		{
			m_CameraController.OnUpdate(ts);
		}

		m_EditorCamera.OnUpdate(ts);

		//Render
		Renderer2D::ResetStats();
		m_Framebuffer->Bind();
		RenderCommand::SetClearColor({ 0.07f, 0.07f, 0.07f, 1.0f });
		RenderCommand::Clear();

		m_ActiveScene->OnUpdateEditor(ts, m_EditorCamera);

		m_Framebuffer->UnBind();

	}

	void EditorLayer::OnImGuiRender()
	{
		DME_PROFILE_FUNCTION();

		ImGui::ShowDemoWindow();
		EditorLayer::OnDockspace();

		m_SceneHierarchy.OnImGuiRender();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(3, 3));
		ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);


		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();

		Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused && !m_ViewportHovered);

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

		ImGui::Image(reinterpret_cast<void*>(static_cast<uint64_t>(m_Framebuffer->GetColorAttachmentRendererID())), ImVec2(m_ViewportSize.x, m_ViewportSize.y), { 0, 1 }, { 1, 0 });

		ImGui::SetNextWindowPos(ImVec2(ImGui::GetWindowPos().x + 15, ImGui::GetWindowPos().y + 40));
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5f);
		if (ImGui::BeginChildFrame(23, ImVec2(130, 35), ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoMove))
		{
			ImGui::PopStyleVar();

			ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[SceneHierarchyPanel::SetFont(OpenSansBold_15)]);

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 0.5f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.2f, 0.2f, 0.5f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.2f, 0.2f, 0.5f));
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(9, 5));
			if (ImGui::Button("M", ImVec2(25, 25))) m_GizmoType = -1; ImGui::SameLine();
			if (ImGui::Button("T", ImVec2(25, 25))) m_GizmoType = ImGuizmo::OPERATION::TRANSLATE; ImGui::SameLine();
			if (ImGui::Button("R", ImVec2(25, 25))) m_GizmoType = ImGuizmo::OPERATION::ROTATE; ImGui::SameLine();
			if (ImGui::Button("S", ImVec2(25, 25))) m_GizmoType = ImGuizmo::OPERATION::SCALE;
			ImGui::PopStyleVar();

			ImGui::PopStyleColor(4);

			ImGui::PopFont();

			ImGui::EndChildFrame();

		}


		// Gizmos

		Entity selectedEntity = m_SceneHierarchy.GetSelectedEntity();
		if (selectedEntity && m_GizmoType != -1)
		{
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();

			float windowWidth = (float)ImGui::GetWindowWidth();
			float windowHeight = (float)ImGui::GetWindowHeight();
			ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

			// Camera
			// auto cameraEntity = m_ActiveScene->GetPrimaryCameraEntity();
			// const auto& camera = cameraEntity.GetComponent<CameraComponent>().Camera;
			// const glm::mat4& cameraProjection = camera.GetProjection();
			// glm::mat4 cameraView = glm::inverse(cameraEntity.GetComponent<TransformComponent>().GetTransform());

			// Editor Camera
			const glm::mat4& cameraProjection = m_EditorCamera.GetProjection();
			glm::mat4 cameraView = m_EditorCamera.GetViewMatrix();

			// Entity transform
			auto& tc = selectedEntity.GetComponent<TransformComponent>();
			glm::mat4 transform = tc.GetTransform();

			// Snapping
			bool snap = Input::IsKeyPressed(Key::LeftControl);
			float snapValue = 0.5f; // Snap to 0.5m for translation/scale
			// Snap to 45 degrees for rotation
			if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
				snapValue = 45.0f;

			float snapValues[3] = { snapValue, snapValue, snapValue };

			ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
				(ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform),
				nullptr, snap ? snapValues : nullptr);

			if (ImGuizmo::IsUsing())
			{
				glm::vec3 translation, rotation, scale;
				math::DecomposeTransform(transform, translation, rotation, scale);

				glm::vec3 deltaRotation = rotation - tc.Rotation;
				tc.Position = translation;
				tc.Rotation += deltaRotation;
				tc.Scale = scale;
			}
		}

		ImGui::End();
		ImGui::PopStyleVar();
		
	}

	void EditorLayer::OnDockspace()
	{
		static bool dockspaceOpen = true;
		static bool opt_fullscreen = true;
		static bool opt_padding = false;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}
		else
		{
			dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
		}

		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		if (!opt_padding)
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
		if (!opt_padding)
			ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// Submit the DockSpace
		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		float minWindowSizeX = style.WindowMinSize.x = 290.0f;
		float minWindowSizeY = style.WindowMinSize.y = 100.0f;
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		style.WindowMinSize.x = minWindowSizeX;
		style.WindowMinSize.y = minWindowSizeY;

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New", "Ctrl+N"))
					NewScene();

				ImGui::Separator();

				if (ImGui::MenuItem("Open...", "Ctrl+O"))
					OpenScene();

				ImGui::Separator();

				if (ImGui::MenuItem("Save as...", "Ctrl+Shift+A"))
					SaveSceneAs();

				ImGui::Separator();

				if (ImGui::MenuItem("Exit")) Application::Get().Close();

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		ImGui::End();
	}

	void EditorLayer::OnEvent(Event& event)
	{
		m_CameraController.OnEvent(event);
		m_EditorCamera.OnEvent(event);

		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<KeyPressedEvent>(DME_BIND_EVENT_FN(EditorLayer::OnKeyPressed));

	}

	bool EditorLayer::OnKeyPressed(KeyPressedEvent& event)
	{
		if (event.GetRepeatCount() > 0)
			return false;

		bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
		bool super = Input::IsKeyPressed(Key::LeftSuper) || Input::IsKeyPressed(Key::RightSuper);
		bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);
		bool alt = Input::IsKeyPressed(Key::LeftAlt) || Input::IsKeyPressed(Key::RightAlt);

		switch (event.GetKeyCode())
		{
			case Key::N:
			{
				if (control)
					NewScene();

				break;
			}

			case Key::O:
			{
				if (control)
					OpenScene();

				break;
			}

			case Key::A:
			{
				if (control && shift)
					SaveSceneAs();

				break;
			}



			case Key::Q:
				m_GizmoType = -1;
				break;

			case Key::W:
				m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
				break;

			case Key::E:
				m_GizmoType = ImGuizmo::OPERATION::SCALE;
				break;

			case Key::R:
				m_GizmoType = ImGuizmo::OPERATION::ROTATE;
				break;

		}

		return false;
	}

	void EditorLayer::NewScene()
	{
		m_ActiveScene = CreateRef<Scene>();
		m_ActiveScene->OnViewportResize(static_cast<uint32_t>(m_ViewportSize.x), static_cast<uint32_t>(m_ViewportSize.y));
		m_SceneHierarchy.SetContext(m_ActiveScene);
	}

	void EditorLayer::OpenScene()
	{
		std::string filepath = FileDialogs::OpenFile("DME Scene (*.dme)\0*.dme\0");
		if (!filepath.empty())
		{

			m_ActiveScene = CreateRef<Scene>();
			m_ActiveScene->OnViewportResize(static_cast<uint32_t>(m_ViewportSize.x), static_cast<uint32_t>(m_ViewportSize.y));
			m_SceneHierarchy.SetContext(m_ActiveScene);

			SceneSerializer serializer(m_ActiveScene);
			serializer.Deserialize(filepath);
		}
	}

	void EditorLayer::SaveSceneAs()
	{
		std::string filepath = FileDialogs::SaveFile("DME Scene (*.dme)\0*.dme\0");
		if (!filepath.empty())
		{
			SceneSerializer serializer(m_ActiveScene);
			serializer.Serialize(filepath);
		}
	}

}

