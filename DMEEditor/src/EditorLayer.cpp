#include "dmepch.h"



#include "EditorLayer.h"
#include <ImGui/imgui.h>
#include <ImGui/imgui_internal.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "DME/Scene/SceneSerializer.h"

#include "DME/Utils/PlatformUtils.h"

#include "DME/Renderer/DebugRendererMode.h"

#include "Panels/FontLibrary.h"

#include "ImGuizmo.h"

#include "DME/Math/Math.h"

namespace DME
{

	extern const std::filesystem::path g_AssetPath;

	EditorLayer::EditorLayer() : Layer("EditorLayer"), m_CameraController(1600.0f / 900.0f) { }

	void EditorLayer::OnAttach()
	{

		DME_PROFILE_FUNCTION();

		m_IconPlay = Texture2D::Create("Resources/Icons/Viewport/PlayButton.png");
		m_IconSimulate = Texture2D::Create("Resources/Icons/Viewport/SimulateButton.png");
		m_IconStop = Texture2D::Create("Resources/Icons/Viewport/StopButton.png");

		FramebufferSpecification fbSpec;
		fbSpec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		fbSpec.Width = 1600;
		fbSpec.Height = 900;
		m_Framebuffer = Framebuffer::Create(fbSpec);

		m_EditorScene = CreateRef<Scene>();
		m_ActiveScene = m_EditorScene;

		auto commandLineArgs = Application::Get().GetSpecification().CommandLineArgs;
		if (commandLineArgs.Count > 1)
		{
			auto sceneFilePath = commandLineArgs[1];
			SceneSerializer serializer(m_ActiveScene);
			serializer.Deserialize(sceneFilePath);
		}

		m_EditorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);

		Renderer2D::SetLineWidth(4.0f);
		
	}

	void EditorLayer::OnDetach()
	{
		DME_PROFILE_FUNCTION();

	}

	void EditorLayer::OnUpdate(TimeStep ts)
	{
		DME_PROFILE_FUNCTION();

		if (m_ViewportFocused)
			m_EditorCamera.SetViewportActive(true);
		else
		{
			m_EditorCamera.SetViewportActive(false);
		}
		if (FramebufferSpecification spec = m_Framebuffer->GetSpecification(); 
			m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && 
			(spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
		{
			m_Framebuffer->Resize(static_cast<uint32_t>(m_ViewportSize.x), static_cast<uint32_t>(m_ViewportSize.y));
			m_CameraController.OnResize(m_ViewportSize.x, m_ViewportSize.y);
			m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
			m_ActiveScene->OnViewportResize(static_cast<uint32_t>(m_ViewportSize.x), static_cast<uint32_t>(m_ViewportSize.y));

		}
		
		//Render
		Renderer2D::ResetStats();
		m_Framebuffer->Bind();
		RenderCommand::SetClearColor({ 0.07f, 0.07f, 0.07f, 1.0f });
		RenderCommand::Clear();

		m_Framebuffer->ClearAttachment(1, -1);

		switch (m_SceneState)
		{
			case SceneState::Edit:
			{
				if (m_ViewportFocused)
					m_CameraController.OnUpdate(ts);

				m_EditorCamera.OnUpdate(ts);

				m_ActiveScene->OnUpdateEditor(ts, m_EditorCamera);
				break;
			}
			case SceneState::Simulate:
			{
				m_EditorCamera.OnUpdate(ts);

				m_ActiveScene->OnUpdateSimulation(ts, m_EditorCamera);
				break;
			}
			case SceneState::Play:
			{
				m_ActiveScene->OnUpdateRuntime(ts);
				break;
			}
		}

		auto [mx, my] = ImGui::GetMousePos();
		mx -= m_ViewportBounds[0].x;
		my -= m_ViewportBounds[0].y;
		glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
		my = viewportSize.y - my;
		int mouseX = static_cast<int>(mx);
		int mouseY = static_cast<int>(my);

		if (mouseX >= 0 && mouseY >= 0 && mouseX < static_cast<int>(viewportSize.x) && mouseY < static_cast<int>(viewportSize.y))
		{
			int pixelData = m_Framebuffer->ReadPixel(1, mouseX, mouseY);
			m_HoveredEntity = pixelData == -1 ? Entity() : Entity(static_cast<entt::entity>(pixelData), m_ActiveScene.get());
		}

		if (Input::IsKeyPressed(Key::F1))
		{
			s_DebugRendererMode = DebugRendererMode::Normal;
		}
		else if (Input::IsKeyPressed(Key::F2))
		{
			s_DebugRendererMode = DebugRendererMode::Wireframe;
		}
		else if (Input::IsKeyPressed(Key::F3))
		{
			s_DebugRendererMode = DebugRendererMode::Point;
		}

		OnOverlayRender();

		m_Framebuffer->UnBind(); 

	}

	void EditorLayer::OnImGuiRender()
	{
		DME_PROFILE_FUNCTION();

		EditorLayer::OnDockspace();

		if (m_SceneHierarchyWindow)
			m_SceneHierarchy.OnImGuiRender();
		if (m_ContentBrowserWindow)
			m_ContentBrowser.OnImGuiRender();

		if (m_ViewportWindow)
			ViewportWindow();
		if (m_DebugWindow)
			DebugWindow();
		if (m_DemoWindow)
			ImGui::ShowDemoWindow();
		if (m_RendererStatsWindow)
			RendererStatsWindow();

	}

	void EditorLayer::UIToolbar()
	{
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.11f, 0.11f, 0.11f, 0.5f));
		ImGui::SetNextWindowPos(ImVec2(static_cast<float>(ImGui::GetWindowPos().x + (ImGui::GetWindowContentRegionMax().x * 0.5 - 70.0f)), static_cast<float>(ImGui::GetWindowPos().y + 40)));
		ImGui::BeginChild("UI panel", ImVec2(130, 35), ImGuiChildFlags_FrameStyle, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
		
		{
			Ref<Texture2D> icon = (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Simulate) ? m_IconPlay : m_IconStop;
			ImTextureID buttonID = static_cast<uintptr_t>(icon->GetRendererID());
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
			ImGui::SetCursorPosY(ImGui::GetWindowContentRegionMax().y * 0.5f - 12);
			std::string ButtonID = std::format("Image | {0}", buttonID);
			if (ImGui::ImageButton(ButtonID.c_str(), buttonID, ImVec2(24, 24), ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0)))
			{
				if (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Simulate)
					OnScenePlay();
				else if (m_SceneState == SceneState::Play)
					OnSceneStop();
			}
			ImGui::PopStyleVar();

		}
		
		ImGui::SameLine();

		{
			
			Ref<Texture2D> icon = (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Play) ? m_IconSimulate : m_IconStop;
			ImTextureID buttonID = static_cast<uintptr_t>(icon->GetRendererID());
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
			ImGui::SetCursorPosY(ImGui::GetWindowContentRegionMax().y * 0.5f - 12);
			std::string ButtonID = std::format("Image | {0}", buttonID);
			if (ImGui::ImageButton(ButtonID.c_str(), buttonID, ImVec2(24, 24), ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0)))
			{
				if (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Play)
					OnSceneSimulate();
				else if (m_SceneState == SceneState::Simulate)
					OnSceneStop();
			}
			ImGui::PopStyleVar();
			
		}

		ImGui::PopStyleColor();
		ImGui::EndChild();

		bool toolbarEnabled = (bool)m_ActiveScene;

		ImVec4 tintColor = ImVec4(1, 1, 1, 1);
		if (!toolbarEnabled)
			tintColor.w = 0.5f;

	}
	void EditorLayer::GizmosToolbar()
	{
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.11f, 0.11f, 0.11f, 0.5f));
		ImGui::SetNextWindowPos(ImVec2(ImGui::GetWindowPos().x + 15, ImGui::GetWindowPos().y + 40));
		if (ImGui::BeginChild("Control panel", ImVec2(130, 35), ImGuiChildFlags_FrameStyle, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
		{
			ImGui::PopStyleColor();

			ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[FontLibrary::OpenSansBold_21]);

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 0.5f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.2f, 0.2f, 0.5f));
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(9, 5));
			ImGui::PushStyleColor(ImGuiCol_Button, m_GizmoType == -1 ? ImVec4(0.2f, 0.6f, 0.9f, 0.5f) : ImVec4(0.2f, 0.2f, 0.2f, 0.5f));
			if (ImGui::Button("M", ImVec2(25, 25))) m_GizmoType = -1; ImGui::SameLine();
			ImGui::PopStyleColor();
			ImGui::PushStyleColor(ImGuiCol_Button, m_GizmoType == ImGuizmo::OPERATION::TRANSLATE ? ImVec4(0.2f, 0.6f, 0.9f, 0.5f) : ImVec4(0.2f, 0.2f, 0.2f, 0.5f));
			if (ImGui::Button("T", ImVec2(25, 25))) m_GizmoType = ImGuizmo::OPERATION::TRANSLATE; ImGui::SameLine();
			ImGui::PopStyleColor();
			ImGui::PushStyleColor(ImGuiCol_Button, m_GizmoType == ImGuizmo::OPERATION::ROTATE ? ImVec4(0.2f, 0.6f, 0.9f, 0.5f) : ImVec4(0.2f, 0.2f, 0.2f, 0.5f));
			if (ImGui::Button("R", ImVec2(25, 25))) m_GizmoType = ImGuizmo::OPERATION::ROTATE; ImGui::SameLine();
			ImGui::PopStyleColor();
			ImGui::PushStyleColor(ImGuiCol_Button, m_GizmoType == ImGuizmo::OPERATION::SCALE ? ImVec4(0.2f, 0.6f, 0.9f, 0.5f) : ImVec4(0.2f, 0.2f, 0.2f, 0.5f));
			if (ImGui::Button("S", ImVec2(25, 25))) m_GizmoType = ImGuizmo::OPERATION::SCALE;
			ImGui::PopStyleColor();
			ImGui::PopStyleVar();
			ImGui::PopStyleColor(3);

			ImGui::PopFont();

			ImGui::EndChild();

		}
	}
	void EditorLayer::UITabBar()
	{
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.2f, 0.5f, 0.8f, 0.7f));
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


				if (ImGui::MenuItem("Save", "Ctrl+S"))
					SaveScene();

				if (ImGui::MenuItem("Save as...", "Ctrl+Shift+A"))
					SaveSceneAs();

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Edit"))
			{

				if (ImGui::MenuItem("Exit")) Application::Get().Close();

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Window"))
			{
				ImGui::SeparatorText("Main");
				if (ImGui::MenuItem("Viewport", nullptr, &m_ViewportWindow));
				if (ImGui::MenuItem("Scene Hierarchy", nullptr, &m_SceneHierarchyWindow));

				ImGui::SeparatorText("Other");
				if (ImGui::MenuItem("Settings", nullptr, &m_SettingsWindow));
				if (ImGui::MenuItem("Debug", nullptr, &m_DebugWindow));
				if (ImGui::MenuItem("Demo", nullptr, &m_DemoWindow));
				if (ImGui::MenuItem("Renderer Stats", nullptr, &m_RendererStatsWindow));

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Mode"))
			{
				ImGui::SeparatorText("View mode");
				if (ImGuiDMEEditor::MenuItemEx("Default", nullptr, nullptr, s_DebugRendererMode == DebugRendererMode::Normal)) s_DebugRendererMode = DebugRendererMode::Normal;
				if (ImGuiDMEEditor::MenuItemEx("Wireframe", nullptr, nullptr, s_DebugRendererMode == DebugRendererMode::Wireframe)) s_DebugRendererMode = DebugRendererMode::Wireframe;
				if (ImGuiDMEEditor::MenuItemEx("Point", nullptr, nullptr, s_DebugRendererMode == DebugRendererMode::Point)) s_DebugRendererMode = DebugRendererMode::Point;

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
			ImGui::PopStyleColor();
		}
	}

	void EditorLayer::SerializeScene(Ref<Scene> scene, const std::filesystem::path& path)
	{
		SceneSerializer serializer(scene);
		serializer.Serialize(path.string());
	}

	void EditorLayer::OnScenePlay()
	{
		if (m_SceneState == SceneState::Simulate)
			OnSceneStop();

		m_SceneState = SceneState::Play;

		m_ActiveScene = Scene::Copy(m_EditorScene);
		m_ActiveScene->OnRuntimeStart();

		m_SceneHierarchy.SetContext(m_ActiveScene);
	}

	void EditorLayer::OnSceneSimulate()
	{
		if (m_SceneState == SceneState::Play)
			OnSceneStop();

		m_SceneState = SceneState::Simulate;

		m_ActiveScene = Scene::Copy(m_EditorScene);
		m_ActiveScene->OnSimulationStart();

		m_SceneHierarchy.SetContext(m_ActiveScene);
	}

	void EditorLayer::OnSceneStop()
	{
		DME_CORE_ASSERT(m_SceneState == SceneState::Play || m_SceneState == SceneState::Simulate);

		if (m_SceneState == SceneState::Play)
			m_ActiveScene->OnRuntimeStop();
		else if (m_SceneState == SceneState::Simulate)
			m_ActiveScene->OnSimulationStop();

		m_SceneState = SceneState::Edit;

		m_ActiveScene = m_EditorScene;

		m_SceneHierarchy.SetContext(m_ActiveScene);

	}

	void EditorLayer::OnDuplicateEntity()
	{
		if (m_SceneState != SceneState::Edit)
			return;

		Entity selectedEntity = m_SceneHierarchy.GetSelectedEntity();
		if (selectedEntity)
			m_EditorScene->DuplicateEntity(selectedEntity);
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

		UITabBar();

		ImGui::End();
	}

	void EditorLayer::OnEvent(Event& event)
	{

		if (m_ViewportHoveredAndFocused)
		{
			m_CameraController.OnEvent(event);
			if (m_SceneState == SceneState::Edit)
			{
				m_EditorCamera.OnEvent(event);
			}
		}

		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<KeyPressedEvent>(DME_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
		dispatcher.Dispatch<MouseButtonPressedEvent>(DME_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));

	}

	bool EditorLayer::OnKeyPressed(KeyPressedEvent& event)
	{
		if (event.IsRepeat())
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
			case Key::S:
			{
				if (control)
				{
					if (shift)
						SaveSceneAs();
					else
						SaveScene();
				}

				break;
			}

			// Scene Commands
			case Key::D:
			{
				if (control)
					OnDuplicateEntity();

				break;
			}

			if (m_ViewportFocused && m_SceneState == SceneState::Edit)
			{
				case Key::Q:
				{
					if (!ImGuizmo::IsUsing())
						m_GizmoType = -1;
					break;
				}
					
				case Key::W:
				{
					if (!ImGuizmo::IsUsing())
						m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
					break;
				}
				
				case Key::E:
				{
					if (!ImGuizmo::IsUsing())
						m_GizmoType = ImGuizmo::OPERATION::ROTATE;
					break;
				}

				case Key::R:
				{
					if (!ImGuizmo::IsUsing())
						m_GizmoType = ImGuizmo::OPERATION::SCALE;
					break;
				}
			}
		}

		return false;
	}

	bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		if (e.GetMouseButton() == Mouse::ButtonLeft)
		{
			if (m_ViewportHovered && !ImGuizmo::IsOver() && !Input::IsKeyPressed(Key::LeftShift))
				m_SceneHierarchy.SetSelectedEntity(m_HoveredEntity);
		}
		return false;
	}

	void EditorLayer::OnOverlayRender()
	{
		if (m_SceneState == SceneState::Play)
		{
			Entity camera = m_ActiveScene->GetPrimaryCameraEntity();
			if (!camera)
				return;
			Renderer2D::BeginScene(camera.GetComponent<CameraComponent>().Camera, camera.GetComponent<TransformComponent>().GetTransform());
		}
		else
		{
			Renderer2D::BeginScene(m_EditorCamera);
		}

		if (Entity selectedEntity = m_SceneHierarchy.GetSelectedEntity())
		{

			if (selectedEntity.HasComponent<BoxCollider2DComponent>())
			{
				const TransformComponent& transform = selectedEntity.GetComponent<TransformComponent>();
				const BoxCollider2DComponent& bc2d = selectedEntity.GetComponent<BoxCollider2DComponent>();

				glm::vec3 translation = transform.Position + glm::vec3(bc2d.Offset, 0.001f);
				glm::vec3 scale = transform.Scale * glm::vec3(bc2d.Size * 2.0f, 1.0f);

				glm::mat4 position = glm::translate(glm::mat4(1.0f), translation)
					* glm::rotate(glm::mat4(1.0f), transform.Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f))
					* glm::scale(glm::mat4(1.0f), scale);

				Renderer2D::DrawRect(position, glm::vec4(1.0f, 0.5f, 0.0f, 1.0f));
			}

			if (selectedEntity.HasComponent<CircleCollider2DComponent>())
			{
				const TransformComponent& transform = selectedEntity.GetComponent<TransformComponent>();
				const CircleCollider2DComponent& cc2d = selectedEntity.GetComponent<CircleCollider2DComponent>();


				glm::vec3 translation = transform.Position + glm::vec3(cc2d.Offset, 0.001f);
				glm::vec3 scale = transform.Scale * glm::vec3(cc2d.Radius * 2.5f);

				glm::mat4 position = glm::translate(glm::mat4(1.0f), translation) * glm::scale(glm::mat4(1.0f), scale);

				Renderer2D::DrawCircle(position, glm::vec4(1.0f, 0.5f, 0.0f, 1.0f), 0.15f);

			}
		}

		Renderer2D::EndScene();
	}

	void EditorLayer::NewScene()
	{
		m_ActiveScene = CreateRef<Scene>();
		m_ActiveScene->OnViewportResize(static_cast<uint32_t>(m_ViewportSize.x), static_cast<uint32_t>(m_ViewportSize.y));
		m_SceneHierarchy.SetContext(m_ActiveScene);

		m_EditorScenePath = std::filesystem::path();
	}

	void EditorLayer::OpenScene()
	{
		std::string filepath = FileDialogs::OpenFile("DME Scene (*.dme)\0*.dme\0");
		if (!filepath.empty())
			OpenScene(filepath);
	}

	void EditorLayer::OpenScene(const std::filesystem::path& path)
	{
		if (m_SceneState != SceneState::Edit)
			OnSceneStop();

		if (path.extension().string() != ".dme")
		{
			DME_WARNING("Could not load {0} - not a scene file", path.filename().string());
			return;
		}

		Ref<Scene> newScene = CreateRef<Scene>();
		SceneSerializer serializer(newScene);
		if (serializer.Deserialize(path.string()))
		{
			m_EditorScene = newScene;
			m_EditorScene->OnViewportResize(static_cast<uint32_t>(m_ViewportSize.x), static_cast<uint32_t>(m_ViewportSize.y));
			m_SceneHierarchy.SetContext(m_EditorScene);

			m_ActiveScene = m_EditorScene;
			m_EditorScenePath = path;
		}
	}

	void EditorLayer::SaveScene()
	{
		if (!m_EditorScenePath.empty())
			SerializeScene(m_ActiveScene, m_EditorScenePath);
		else
			SaveSceneAs();
	}

	void EditorLayer::SaveSceneAs()
	{
		std::string filepath = FileDialogs::SaveFile("DME Scene (*.dme)\0*.dme\0");
		if (!filepath.empty())
		{
			SerializeScene(m_ActiveScene, filepath);
			m_EditorScenePath = filepath;
		}
	}

	void EditorLayer::ViewportWindow()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(3.5f, 3.5f));
		ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoCollapse);
		ImGui::PopStyleVar();

		auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
		auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		auto viewportOffset = ImGui::GetWindowPos();
		m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
		m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHoveredAndFocused = ImGui::IsWindowFocused() && ImGui::IsWindowHovered();
		m_ViewportHovered = ImGui::IsWindowHovered();
		m_ViewportDocked = ImGui::IsWindowDocked();

		Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused && !m_ViewportHovered);

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

		uint64_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
		ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2(m_ViewportSize.x, m_ViewportSize.y), { 0, 1 }, { 1, 0 });

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				const wchar_t* path = (const wchar_t*)payload->Data;
				OpenScene(std::filesystem::path(g_AssetPath) / path);
				DME_CORE_INFO("Deserialize");
			}
			ImGui::EndDragDropTarget();
		}

		GizmosToolbar();
		UIToolbar();

		Entity selectedEntity = m_SceneHierarchy.GetSelectedEntity();
		if (selectedEntity && m_GizmoType != -1)
		{
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();
			ImGuizmo::SetRect(m_ViewportBounds[0].x, m_ViewportBounds[0].y, m_ViewportBounds[1].x - m_ViewportBounds[0].x, m_ViewportBounds[1].y - m_ViewportBounds[0].y);

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
	}

	void EditorLayer::DebugWindow()
	{
		ImGui::Begin("Debug Window");

		ImGui::Text("Focus: %s\nHover: %s\nDock: %s", m_ViewportFocused ? "Focused" : "No", m_ViewportHovered ? "Hovered" : "No", m_ViewportDocked ? "Docked" : "No");
		ImGui::Text("Debug mode: %s", DebugModeToString(s_DebugRendererMode).c_str());
		ImGui::Text("Viewport for EC: %s", m_EditorCamera.GetViewportStatusAsString().c_str());

		ImGui::End();
	}

	void EditorLayer::RendererStatsWindow()
	{
		ImGui::Begin("Renderer Stats");

		ImGui::Text("Draw Calls: %d", Renderer2D::GetStats().DrawCalls);
		ImGui::Text("Quads: %d", Renderer2D::GetStats().QuadCount);
		ImGui::Text("Vertices: %d", Renderer2D::GetStats().GetTotalVertexCount());
		ImGui::Text("Indices: %d", Renderer2D::GetStats().GetTotalIndexCount());
		ImGui::Text("FPS: %d", static_cast<uint16_t>(ImGui::GetIO().Framerate));
		ImGui::Text("Frame time: %.3f ms", static_cast<uint16_t>(ImGui::GetIO().Framerate) / 1000.0f);

		std::string name = m_HoveredEntity && m_HoveredEntity.HasComponent<TagComponent>() ? m_HoveredEntity.GetComponent<TagComponent>().Tag : "None";
		ImGui::Text("Hovered Entity: %s", name.c_str());

		ImGui::End();
	}

}