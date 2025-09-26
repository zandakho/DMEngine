#include "dmepch.h"

#include "EditorLayer.h"

#include "DME/Scene/SceneSerializer.h"

#include "DME/Utils/PlatformUtils.h"

#include "DME/Math/Math.h"

#include <ImGui/imgui.h>
#include <ImGui/imgui_internal.h>
#include <ImGuizmo.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GLFW/include/GLFW/glfw3.h"

namespace DME
{

	extern const std::filesystem::path g_AssetPath;

	// Constructors and destructors
	EditorLayer::EditorLayer() : Layer("EditorLayer"), m_CameraController(1600.0f / 900.0f) {}

	// Layer overrides
	void EditorLayer::OnAttach()
	{
		DME_PROFILE_FUNCTION()

		m_ConsolePanel.OnAttach();
		m_ContentBrowserPanel.OnAttach();
		m_SceneHierarchyPanel.OnAttach();
		m_PropertiesPanel.OnAttach();

		m_IconPlay = Texture2D::Create("Resources/Icons/Viewport/Viewport_PlayIcon_Img.png");
		m_IconSimulate = Texture2D::Create("Resources/Icons/Viewport/Viewport_SimulateIcon_Img.png");
		m_IconStop = Texture2D::Create("Resources/Icons/Viewport/Viewport_StopIcon_Img.png");

		m_IconCursor = Texture2D::Create("Resources/Icons/Viewport/Viewport_CursorIcon_Img.png");
		m_IconMove = Texture2D::Create("Resources/Icons/Viewport/Viewport_MoveIcon_Img.png");
		m_IconRotate = Texture2D::Create("Resources/Icons/Viewport/Viewport_RotateIcon_Img.png");
		m_IconScale = Texture2D::Create("Resources/Icons/Viewport/Viewport_ResizeIcon_Img.png");

		m_IconLocal = Texture2D::Create("Resources/Icons/Viewport/Viewport_LocalIcon_Img.png");
		m_IconWorld = Texture2D::Create("Resources/Icons/Viewport/Viewport_WorldIcon_Img.png");

		m_IconSave = Texture2D::Create("Resources/Icons/Editor/Editor_SaveIcon_Img.png");

		m_IconWindow = Texture2D::Create("Resources/Icons/Window/Window_WindowIcon_Img.png");
		m_IconWindowClose = Texture2D::Create("Resources/Icons/Window/Window_WindowCloseIcon_Img.png");
		m_IconWindowFullscreen = Texture2D::Create("Resources/Icons/Window/Window_WindowFullscreenIcon_Img.png");
		m_IconWindowNoFullscreen = Texture2D::Create("Resources/Icons/Window/Window_WindowNoFullscreenIcon_Img.png");
		m_IconWindowMinimize = Texture2D::Create("Resources/Icons/Window/Window_WindowMinimizeIcon_Img.png");

		m_DRM_IconDefault = Texture2D::Create("Resources/Icons/DebugRenderMode/DRM_DefaultIcon_Img.png");
		m_DRM_IconWireframe = Texture2D::Create("Resources/Icons/DebugRenderMode/DRM_WireframeIcon_Img.png");
		m_DRM_IconPoint = Texture2D::Create("Resources/Icons/DebugRenderMode/DRM_PointIcon_Img.png");


		FramebufferSpecification fbSpec;
		fbSpec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		fbSpec.Width = 1600;
		fbSpec.Height = 900;
		m_Framebuffer = Framebuffer::Create(fbSpec);

		m_EditorScene = CreateRef<Scene>();
		m_ActiveScene = m_EditorScene;

		m_SceneHierarchyPanel.CreateSceneIfEmpty(m_EditorScene);

		auto commandLineArgs = Application::Get().GetSpecification().CommandLineArgs;
		if (commandLineArgs.Count > 1)
		{
			auto sceneFilePath = commandLineArgs[1];
			SceneSerializer serializer(m_ActiveScene);
			serializer.Deserialize(sceneFilePath);
		}

		m_EditorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);

		Renderer2D::SetLineWidth(2.0f);

	}

	void EditorLayer::OnDetach()
	{
		DME_PROFILE_FUNCTION()

		m_ConsolePanel.OnDetach();
		m_ContentBrowserPanel.OnDetach();
		m_SceneHierarchyPanel.OnDetach();
		m_PropertiesPanel.OnDetach();
	}

	void EditorLayer::OnUpdate(TimeStep ts)
	{
		DME_PROFILE_FUNCTION()

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

		Renderer2D::ResetStats();
		m_Framebuffer->Bind();
		RenderCommand::SetClearColor({ 0.07f, 0.07f, 0.07f, 1.0f });
		RenderCommand::Clear();

		m_Framebuffer->ClearAttachment(1, -1);


		SwitchSceneStateOnUpdate(ts);

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

		OnOverlayRender();

		m_Framebuffer->UnBind();

	}

	void EditorLayer::OnImGuiRender()
	{
		DME_PROFILE_FUNCTION()

		EditorLayer::OnDockspace();

		ImGui::BeginDisabled(m_SceneState == SceneState::Play);

		if (m_ConsolePanel.m_ConsoleRender) m_ConsolePanel.OnImGuiRender();

		if (m_ContentBrowserPanel.m_ContentBrowserRender) m_ContentBrowserPanel.OnImGuiRender();

		if (m_SceneHierarchyPanel.m_SceneHierarchyRender) m_SceneHierarchyPanel.OnImGuiRender();

		if (m_PropertiesPanel.m_PropertiesPanelRender)
		{
			m_PropertiesPanel.SetContext(m_SceneHierarchyPanel.GetSelectedEntity());
			m_PropertiesPanel.OnImGuiRender();
		}

		ImGui::EndDisabled();

		if (m_ViewportWindow)
			ViewportWindow();
		if (m_DebugWindow)
			DebugWindow();
		if (m_DemoWindow)
			ImGui::ShowDemoWindow();
		if (m_RendererStatsWindow)
			RendererStatsWindow();
	}

	void EditorLayer::OnDockspace()
	{
		ImGui::SetNextWindowPos(ImGui::GetMainViewport()->WorkPos);
		ImGui::SetNextWindowSize({ ImGui::GetMainViewport()->WorkSize.x, 70 });
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, Application::Get().IsWindowMaximized() ? ImVec2(7, 7) : ImVec2(0, 0));
		ImGui::PushStyleVarY(ImGuiStyleVar_FramePadding, 13);
		ImGui::PushStyleColor(ImGuiCol_MenuBarBg, IM_COL32(0, 0, 0, 0));
		ImGui::Begin("TitleBar", nullptr, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
		ImGui::PopStyleColor();
		ImGui::PopStyleVar(2);

		ImVec2 window_pos = ImGui::GetWindowPos();
		ImVec2 window_size = ImGui::GetWindowSize();

		ImGui::GetWindowDrawList()->AddRectFilledMultiColor(
			window_pos,
			ImVec2(window_pos.x + 500, window_pos.y + window_size.y),
			ImColor(120, 40, 120, 130),
			ImColor(0, 0, 0, 0),
			ImColor(0, 0, 0, 0),
			ImColor(120, 40, 120, 130)
		);
		ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(0, 0, 0, 0));
		ImGui::BeginChild("##TitleBarChildIcon", ImVec2(70, 70), ImGuiChildFlags_None, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_AlwaysUseWindowPadding);

		ImGui::SetCursorPos({0, 0});
		ImGui::Image(m_IconWindow->GetRendererID(), ImVec2(64, 64), { 0, 1 }, { 1, 0 });

		ImGui::EndChild();
		ImGui::PopStyleColor();

		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_MenuBarBg, IM_COL32(0, 0, 0, 0));
		ImGui::SetCursorPosX(70);
		ImGui::BeginChild("##TitleBarChildMenuBar", ImVec2(ImGui::GetContentRegionAvail().x - 85, 70), ImGuiChildFlags_None, Application::Get().IsWindowMaximized() ? ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse : ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		UITabBar();

		ImGui::EndChild();
		ImGui::PopStyleColor(2);

		ImGui::SameLine();

		ImGui::SetCursorPosX(ImGui::GetContentRegionMax().x - 85);
		ImGui::PushStyleVarX(ImGuiStyleVar_ItemSpacing, 0.0f);
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
		if (ImGuiDMEEditor::IconButton("##MinimizeButton", reinterpret_cast<ImTextureID*>(static_cast<uintptr_t>(m_IconWindowMinimize->GetRendererID())), { 28, 28 })) glfwIconifyWindow(Application::Get().GetNativeWindow());
		ImGui::PopStyleColor();

		ImGui::SameLine();

		Ref<Texture2D> icon = Application::Get().IsWindowMaximized() ? m_IconWindowNoFullscreen : m_IconWindowFullscreen;
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		if (ImGuiDMEEditor::IconButton("##FullscreenButton", reinterpret_cast<ImTextureID*>(static_cast<uintptr_t>(icon->GetRendererID())), { 28, 28 }))
		{
			if (Application::Get().IsWindowMaximized())
				glfwRestoreWindow(Application::Get().GetNativeWindow());
			else glfwMaximizeWindow(Application::Get().GetNativeWindow());
		}
		ImGui::PopStyleColor();

		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.7f, 0.0f, 0.0f, 0.8f));
		if (ImGuiDMEEditor::IconButton("##CloseWindow", reinterpret_cast<ImTextureID*>(static_cast<uintptr_t>(m_IconWindowClose->GetRendererID())), { 28, 28 })) Application::Get().Close();
		ImGui::PopStyleColor(2);
		ImGui::PopStyleVar(2);

		ImGui::PopStyleVar();
		
		ImGui::End();

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;

		ImGui::SetNextWindowPos({ ImGui::GetMainViewport()->WorkPos.x, ImGui::GetMainViewport()->WorkPos.y + 70 });
		ImGui::SetNextWindowSize({ ImGui::GetMainViewport()->WorkSize.x, ImGui::GetMainViewport()->WorkSize.y - 70 });
		ImGui::SetNextWindowViewport(ImGui::GetMainViewport()->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.10f, 0.10f, 0.10f, 1.0f));

		ImGui::Begin("Dockspace", nullptr, window_flags);

		ImGui::PopStyleColor();
		ImGui::PopStyleVar();

		ImGui::PopStyleVar(2);

		// Submit the DockSpace
		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		float minWindowSizeX = style.WindowMinSize.x = 30.0f;
		float minWindowSizeY = style.WindowMinSize.y = 30.0f;
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::PushStyleColor(ImGuiCol_DockingEmptyBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_NoCloseButton);
			ImGui::PopStyleColor();
		}

		style.WindowMinSize.x = minWindowSizeX;
		style.WindowMinSize.y = minWindowSizeY;

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

	// Events
	bool EditorLayer::OnKeyPressed(const KeyPressedEvent& event)
	{
		if (event.IsRepeat())
			return false;

		bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
		bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);

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

			case Key::D:
			{
				if (control)
					OnDuplicateEntity();

				break;
			}

			case Key::Q:
			{
				if (m_ViewportFocused && m_SceneState == SceneState::Edit)
				{
					if (!ImGuizmo::IsUsing())
						m_GizmoType = -1;
				}
				break;
			}

			case Key::W:
			{

				if(m_ViewportFocused && m_SceneState == SceneState::Edit)
				{
					if (!ImGuizmo::IsUsing() && m_GizmoType != ImGuizmo::OPERATION::TRANSLATE)
						m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
				}
				break;
			}

			case Key::E:
			{
				if (m_ViewportFocused && m_SceneState == SceneState::Edit)
				{
					if (!ImGuizmo::IsUsing() && m_GizmoType != ImGuizmo::OPERATION::ROTATE)
						m_GizmoType = ImGuizmo::OPERATION::ROTATE;
				}
				break;
			}

			case Key::R:
			{
				if (m_ViewportFocused && m_SceneState == SceneState::Edit)
				{
					if (!ImGuizmo::IsUsing() && m_GizmoType != ImGuizmo::OPERATION::SCALE)
						m_GizmoType = ImGuizmo::OPERATION::SCALE;
				}
				break;
			}

			case Key::Delete:
			{
				if (m_SceneHierarchyPanel.GetSelectedEntity())
				{
					DME_CORE_WARNING("Delete entity: {0}", m_SceneHierarchyPanel.GetSelectedEntity().GetName())
					DeleteSelectedEntity();
				}
				break;
			}

			default: break;
		}

		m_ConsolePanel.OnKeyPressed(event);
		m_ContentBrowserPanel.OnKeyPressed(event);
		m_SceneHierarchyPanel.OnKeyPressed(event);
		m_PropertiesPanel.OnKeyPressed(event);

		if (m_SceneState == SceneState::Play)
			m_DebugRenderer.OnKeyPressed(event);

		return false;
	}

	bool EditorLayer::OnMouseButtonPressed(const MouseButtonPressedEvent& event)
	{
		switch (event.GetMouseButton())
		{
			case Mouse::ButtonLeft:
			{
				if (m_ViewportFocused && m_ViewportHovered && !ImGuizmo::IsOver() && !Input::IsKeyPressed(Key::LeftShift)) m_SceneHierarchyPanel.SetSelectedEntity(m_HoveredEntity);

			}

			default: break;
		}

		m_ConsolePanel.OnMouseButtonPressed(event);
		m_ContentBrowserPanel.OnMouseButtonPressed(event);
		m_SceneHierarchyPanel.OnMouseButtonPressed(event);
		m_PropertiesPanel.OnMouseButtonPressed(event);


		return false;
	}

	// UI functions

	void EditorLayer::UITabBar()
	{

		ImGui::PushStyleColor(ImGuiCol_MenuBarBg, IM_COL32(0, 0, 0, 0));
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
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
				ImGui::SeparatorText("Main");
				ImGuiDMEEditor::Checkbox("Console##Window", &m_ConsolePanel.m_ConsoleRender);
				ImGuiDMEEditor::Checkbox("ContentBrowser##Window", &m_ContentBrowserPanel.m_ContentBrowserRender);
				ImGuiDMEEditor::Checkbox("Viewport##Window", &m_ViewportWindow);
				ImGuiDMEEditor::Checkbox("Scene Hierarchy##Window", &m_SceneHierarchyPanel.m_SceneHierarchyRender);
				ImGuiDMEEditor::Checkbox("Properties##Window", &m_PropertiesPanel.m_PropertiesPanelRender);

				ImGui::SeparatorText("Other");
				ImGuiDMEEditor::Checkbox("Settings##Window", &m_SettingsWindow);
				ImGuiDMEEditor::Checkbox("Debug##Window", &m_DebugWindow);
				ImGuiDMEEditor::Checkbox("Demo##Window", &m_DemoWindow);
				ImGuiDMEEditor::Checkbox("Renderer Stats##Window", &m_RendererStatsWindow);

				ImGui::PopStyleVar();
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Log"))
			{
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
				ImGui::SeparatorText("Core");
				ImGuiDMEEditor::Checkbox("Core Log", &DME::LogSettings::m_GlobalCoreLogger);
				ImGui::Separator();
				ImGuiDMEEditor::Checkbox("Core Critical", &DME::LogSettings::m_CoreCriticalLogger);
				ImGuiDMEEditor::Checkbox("Core Error", &DME::LogSettings::m_CoreErrorLogger);
				ImGuiDMEEditor::Checkbox("Core Warning", &DME::LogSettings::m_CoreWarningLogger);
				ImGuiDMEEditor::Checkbox("Core Info", &DME::LogSettings::m_CoreInfoLogger);
				ImGuiDMEEditor::Checkbox("Core Trace", &DME::LogSettings::m_CoreTraceLogger);

				ImGui::SeparatorText("Default");
				ImGuiDMEEditor::Checkbox("Default Log", &DME::LogSettings::m_GlobalLogger);
				ImGui::Separator();
				ImGuiDMEEditor::Checkbox("Default Critical", &DME::LogSettings::m_CriticalLogger);
				ImGuiDMEEditor::Checkbox("Default Error", &DME::LogSettings::m_ErrorLogger);
				ImGuiDMEEditor::Checkbox("Default Warning", &DME::LogSettings::m_WarningLogger);
				ImGuiDMEEditor::Checkbox("Default Info", &DME::LogSettings::m_InfoLogger);
				ImGuiDMEEditor::Checkbox("Default Trace", &DME::LogSettings::m_TraceLogger);

				ImGui::PopStyleVar();
				ImGui::EndMenu();
			}

			ImGui::InvisibleButton("##TitleBarDragZone", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetFrameHeight()));
			Application::Get().IsTitleBarHovered(ImGui::IsItemHovered());

			ImGui::SetCursorPosX(ImGui::GetContentRegionMax().x / 2 - (ImGui::CalcTextSize(Application::Get().GetSpecification().Name.c_str()).x / 2));
			ImGui::Text("%s", Application::Get().GetSpecification().Name.c_str());

			ImGui::EndMenuBar();
			ImGui::PopStyleColor(2);

		}

		ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(12, 12, 12, 160));
		ImGui::BeginChild("##OpenSceneChild", !m_EditorScenePath.empty() ? ImVec2(ImGui::CalcTextSize(std::filesystem::path(m_EditorScenePath).stem().string().c_str()).x + ImGui::GetStyle().WindowPadding.x * 2.0f, 40) : ImVec2(ImGui::CalcTextSize("Untitled").x + ImGui::GetStyle().WindowPadding.x * 2.0f, 40), ImGuiChildFlags_FrameStyle, ImGuiWindowFlags_AlwaysUseWindowPadding);
		ImGui::Text("%s", !m_EditorScenePath.empty() ? m_EditorScenePath.stem().string().c_str() : "Untitled");
		ImGui::EndChild();
		ImGui::PopStyleColor();

	}

	void EditorLayer::SwitchSceneStateOnUpdate(TimeStep ts)
	{
		switch (m_SceneState)
		{
			case SceneState::Edit:
			{
				if (m_ViewportFocused)
					m_CameraController.OnUpdate(ts);

				m_EditorCamera.OnUpdate(ts);

				s_DebugRendererMode = DebugRendererMode::Normal;

				m_ActiveScene->OnUpdateEditor(ts, m_EditorCamera);
				break;
			}
			case SceneState::Simulate:
			{
				m_EditorCamera.OnUpdate(ts);

				s_DebugRendererMode = DebugRendererMode::Normal;

				m_ActiveScene->OnUpdateSimulation(ts, m_EditorCamera);
				break;
			}
			case SceneState::Play:
			{
				m_ActiveScene->OnUpdateRuntime(ts);
				break;
			}
		}
	}

	void EditorLayer::OnScenePlay()
	{
		if (m_SceneState == SceneState::Simulate)
			OnSceneStop();

		m_SceneState = SceneState::Play;

		m_ActiveScene = Scene::Copy(m_EditorScene);
		m_ActiveScene->OnRuntimeStart();

		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
		m_PropertiesPanel.SetContext(m_SceneHierarchyPanel.GetSelectedEntity());
	}

	void EditorLayer::OnSceneSimulate()
	{
		if (m_SceneState == SceneState::Play)
			OnSceneStop();

		m_SceneState = SceneState::Simulate;

		m_ActiveScene = Scene::Copy(m_EditorScene);
		m_ActiveScene->OnSimulationStart();

		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
		m_PropertiesPanel.SetContext(m_SceneHierarchyPanel.GetSelectedEntity());
	}

	void EditorLayer::OnSceneStop()
	{
		DME_CORE_ASSERT(m_SceneState == SceneState::Play || m_SceneState == SceneState::Simulate)

		if (m_SceneState == SceneState::Play)
			m_ActiveScene->OnRuntimeStop();
		else if (m_SceneState == SceneState::Simulate)
			m_ActiveScene->OnSimulationStop();

		m_SceneState = SceneState::Edit;

		m_ActiveScene = m_EditorScene;

		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
		m_PropertiesPanel.SetContext(m_SceneHierarchyPanel.GetSelectedEntity());

	}

	// Overlay render
	void EditorLayer::OnOverlayRender() const 
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

		if (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Simulate)
		{
			if (Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity())
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
			m_ActiveScene->UpdateSelectionHighlight(m_SceneHierarchyPanel.GetSelectedEntity());


		}
		Renderer2D::EndScene();
	}

	// Helper scene functions
	void EditorLayer::OnDuplicateEntity() const
	{
		if (m_SceneState != SceneState::Edit)
			return;

		if (const Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity())
		{
			if (selectedEntity)
				m_EditorScene->DuplicateEntity(selectedEntity);
		}
	}

	void EditorLayer::DeleteSelectedEntity()
	{
		if (!m_SceneHierarchyPanel.GetSelectedEntity()) return;

		m_SceneHierarchyPanel.GetContext()->DestroyEntity(m_SceneHierarchyPanel.GetSelectedEntity());
		m_SceneHierarchyPanel.ClearSelectedContext();
	}

	Ref<Texture2D> EditorLayer::SwitchDebugRenderModeIcons(DebugRendererMode mode) const
	{
		switch (mode)
		{
			case DebugRendererMode::Normal: return m_DRM_IconDefault;
			case DebugRendererMode::Wireframe: return m_DRM_IconWireframe;
			case DebugRendererMode::Point: return m_DRM_IconPoint;
		}
	}


	// SceneSerializer functions
	void EditorLayer::NewScene()
	{
		m_ActiveScene = CreateRef<Scene>();
		m_ActiveScene->OnViewportResize(static_cast<uint32_t>(m_ViewportSize.x), static_cast<uint32_t>(m_ViewportSize.y));
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
		m_PropertiesPanel.SetContext(m_SceneHierarchyPanel.GetSelectedEntity());

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
			DME_WARNING("Could not load {0} - not a scene file", path.stem().string().c_str())
			return;
		}

		Ref<Scene> newScene = CreateRef<Scene>();
		SceneSerializer serializer(newScene);
		if (serializer.Deserialize(path.string()))
		{
			m_EditorScene = newScene;
			m_EditorScene->OnViewportResize(static_cast<uint32_t>(m_ViewportSize.x), static_cast<uint32_t>(m_ViewportSize.y));
			m_SceneHierarchyPanel.SetContext(m_EditorScene);

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

	void EditorLayer::SerializeScene(const Ref<Scene>& scene, const std::filesystem::path& path)
	{
		SceneSerializer serializer(scene);
		serializer.Serialize(path.string());
	}

	// Windows
	void EditorLayer::ViewportWindow()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(1.5f, 1.5f));
		ImGui::Begin("Viewport", &m_ViewportWindow, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar);
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
				const wchar_t* path = static_cast<const wchar_t*>(payload->Data);
				OpenScene(std::filesystem::path(g_AssetPath) / path);
				DME_CORE_INFO("Deserialize")
			}
			ImGui::EndDragDropTarget();
		}

		Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
		if (selectedEntity && m_GizmoType != -1)
		{

			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());
			ImGuizmo::SetRect(m_ViewportBounds[0].x, m_ViewportBounds[0].y, m_ViewportBounds[1].x - m_ViewportBounds[0].x, m_ViewportBounds[1].y - m_ViewportBounds[0].y);

			ImGuizmo::SetOverGizmoHotspot(false);

			const glm::mat4& cameraProjection = m_EditorCamera.GetProjection();
			glm::mat4 cameraView = m_EditorCamera.GetViewMatrix();

			auto& tc = selectedEntity.GetComponent<TransformComponent>();
			glm::mat4 transform = tc.GetTransform();

			bool snap = Input::IsKeyPressed(Key::LeftControl);
			float snapValue = (m_GizmoType == ImGuizmo::TRANSLATE) ? 1.0f : 0.1f &&
							  (m_GizmoType == ImGuizmo::ROTATE) ? 10.0f : 0.5f && 
							  (m_GizmoType == ImGuizmo::SCALE) ? 1.0f : 0.5f;
			float snapValues[3] = { snapValue, snapValue, snapValue };

			ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
				static_cast<ImGuizmo::OPERATION>(m_GizmoType), static_cast<ImGuizmo::MODE>(m_GizmoMode), glm::value_ptr(transform),
				nullptr, snap ? snapValues : nullptr);

			if (ImGuizmo::IsUsing())
			{
				glm::vec3 translation, rotation, scale;
				math::DecomposeTransform(transform, translation, rotation, scale);

				if (tc.Position != translation || tc.Rotation != rotation || tc.Scale != scale)
				{
					glm::vec3 translation, rotation, scale;
					math::DecomposeTransform(transform, translation, rotation, scale);
					tc.Position = translation;
					tc.Rotation = rotation;
					tc.Scale = scale;
				}
			}

		}

		ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(10, 10, 10, 140));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(1.5f, 1.5f));
		ImGui::SetCursorPos({ 15, 41 });
		ImGui::BeginChild("##GizmosControl", ImVec2(222, 30), ImGuiChildFlags_AlwaysUseWindowPadding);
		ImGui::PopStyleColor();
		ImGui::PopStyleVar();

		ImGui::PushStyleColor(ImGuiCol_Button, m_GizmoType == -1 ? ImVec4(0.2f, 0.6f, 0.9f, 0.5f) : ImVec4(0.2f, 0.2f, 0.2f, 0.5f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, m_GizmoType == -1 ? ImVec4(0.2f, 0.6f, 0.9f, 0.6f) : ImVec4(0.25f, 0.25f, 0.25f, 0.5f));
		if (ImGuiDMEEditor::IconButton("##Cursor", reinterpret_cast<ImTextureID*>(static_cast<uintptr_t>(m_IconCursor->GetRendererID())), { 26, 26 })) m_GizmoType = -1; ImGui::SameLine();
		ImGui::PopStyleColor(2);

		ImGui::PushStyleColor(ImGuiCol_Button, m_GizmoType == ImGuizmo::OPERATION::TRANSLATE ? ImVec4(0.2f, 0.6f, 0.9f, 0.5f) : ImVec4(0.2f, 0.2f, 0.2f, 0.5f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, m_GizmoType == ImGuizmo::OPERATION::TRANSLATE ? ImVec4(0.2f, 0.6f, 0.9f, 0.6f) : ImVec4(0.25f, 0.25f, 0.25f, 0.5f));
		if (ImGuiDMEEditor::IconButton("##Move", reinterpret_cast<ImTextureID*>(static_cast<uintptr_t>(m_IconMove->GetRendererID())), { 26, 26 })) m_GizmoType = ImGuizmo::OPERATION::TRANSLATE; ImGui::SameLine();
		ImGui::PopStyleColor(2);

		ImGui::PushStyleColor(ImGuiCol_Button, m_GizmoType == ImGuizmo::OPERATION::ROTATE ? ImVec4(0.2f, 0.6f, 0.9f, 0.5f) : ImVec4(0.2f, 0.2f, 0.2f, 0.5f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, m_GizmoType == ImGuizmo::OPERATION::ROTATE ? ImVec4(0.2f, 0.6f, 0.9f, 0.6f) : ImVec4(0.25f, 0.25f, 0.25f, 0.5f));
		if (ImGuiDMEEditor::IconButton("##Rotate", reinterpret_cast<ImTextureID*>(static_cast<uintptr_t>(m_IconRotate->GetRendererID())), { 26, 26 })) m_GizmoType = ImGuizmo::OPERATION::ROTATE; ImGui::SameLine();
		ImGui::PopStyleColor(2);

		ImGui::PushStyleColor(ImGuiCol_Button, m_GizmoType == ImGuizmo::OPERATION::SCALE ? ImVec4(0.2f, 0.6f, 0.9f, 0.5f) : ImVec4(0.2f, 0.2f, 0.2f, 0.5f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, m_GizmoType == ImGuizmo::OPERATION::SCALE ? ImVec4(0.2f, 0.6f, 0.9f, 0.6f) : ImVec4(0.25f, 0.25f, 0.25f, 0.5f));
		if (ImGuiDMEEditor::IconButton("##Scale", reinterpret_cast<ImTextureID*>(static_cast<uintptr_t>(m_IconScale->GetRendererID())), { 26, 26 })) m_GizmoType = ImGuizmo::OPERATION::SCALE;
		ImGui::PopStyleColor(2);

		ImGui::SameLine();

		ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical, 2.0f);

		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, m_GizmoMode == ImGuizmo::LOCAL ? ImVec4(0.2f, 0.6f, 0.9f, 0.5f) : ImVec4(0.2f, 0.2f, 0.2f, 0.5f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, m_GizmoMode == ImGuizmo::LOCAL ? ImVec4(0.2f, 0.6f, 0.9f, 0.6f) : ImVec4(0.25f, 0.25f, 0.25f, 0.5f));
		if (ImGuiDMEEditor::IconButton("##Local", reinterpret_cast<ImTextureID*>(static_cast<uintptr_t>(m_IconLocal->GetRendererID())), { 26, 26 })) m_GizmoMode = ImGuizmo::LOCAL; ImGui::SameLine();
		ImGui::PopStyleColor(2);

		ImGui::PushStyleColor(ImGuiCol_Button, m_GizmoMode == ImGuizmo::WORLD ? ImVec4(0.2f, 0.6f, 0.9f, 0.5f) : ImVec4(0.2f, 0.2f, 0.2f, 0.5f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, m_GizmoMode == ImGuizmo::WORLD ? ImVec4(0.2f, 0.6f, 0.9f, 0.6f) : ImVec4(0.25f, 0.25f, 0.25f, 0.5f));
		if (ImGuiDMEEditor::IconButton("##World", reinterpret_cast<ImTextureID*>(static_cast<uintptr_t>(m_IconWorld->GetRendererID())), { 26, 26 })) m_GizmoMode = ImGuizmo::WORLD;
		ImGui::PopStyleColor(2);

		ImGui::SameLine();

		ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical, 2.0f);

		ImGui::SameLine();

		if (ImGuiDMEEditor::IconButton("##DebugRenderMode", reinterpret_cast<ImTextureID*>(static_cast<uintptr_t>(SwitchDebugRenderModeIcons(s_DebugRendererMode)->GetRendererID())), { 26, 26 })) ImGui::OpenPopup("DebugRenderModePopup");
		if (ImGui::BeginPopup("DebugRenderModePopup", ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoMove))
		{

			ImGui::SeparatorText("View mode");

			ImGui::BeginDisabled(m_SceneState != SceneState::Play);
			if (ImGui::Selectable("Default", s_DebugRendererMode == DebugRendererMode::Normal)) s_DebugRendererMode = DebugRendererMode::Normal;
			if (ImGui::Selectable("Wireframe", s_DebugRendererMode == DebugRendererMode::Wireframe)) s_DebugRendererMode = DebugRendererMode::Wireframe;
			if (ImGui::Selectable("Point", s_DebugRendererMode == DebugRendererMode::Point)) s_DebugRendererMode = DebugRendererMode::Point;
			ImGui::EndDisabled();

			ImGui::EndPopup();
		}

		ImGui::EndChild();

		ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(10, 10, 10, 140));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(1.5f, 1.5f));
		ImGui::SetCursorPos({ ImGui::GetContentRegionAvail().x / 2 - (60.0f / 2), 41 });
		ImGui::BeginChild("##SceneStateControl", ImVec2(60, 30), ImGuiChildFlags_AlwaysUseWindowPadding);
		ImGui::PopStyleColor();
		ImGui::PopStyleVar();

		{
			Ref<Texture2D> icon = (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Simulate) ? m_IconPlay : m_IconStop;
			std::string ButtonID = std::format("Image | {0}", static_cast<uintptr_t>(icon->GetRendererID()));
			if (ImGuiDMEEditor::IconButton(ButtonID.c_str(), reinterpret_cast<ImTextureID*>(static_cast<uintptr_t>(icon->GetRendererID())), { 26, 26 }))
			{
				if (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Simulate)
					OnScenePlay();
				else if (m_SceneState == SceneState::Play)
					OnSceneStop();
			}

		}

		ImGui::SameLine();


		{
			Ref<Texture2D> icon = (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Play) ? m_IconSimulate : m_IconStop;
			std::string ButtonID = std::format("Image | {0}", static_cast<uintptr_t>(icon->GetRendererID()));
			if (ImGuiDMEEditor::IconButton(ButtonID.c_str(), reinterpret_cast<ImTextureID*>(static_cast<uintptr_t>(icon->GetRendererID())), { 26, 26 }))
			{
				if (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Play)
					OnSceneSimulate();
				else if (m_SceneState == SceneState::Simulate)
					OnSceneStop();
			}

		}

		ImGui::EndChild();

		ImGui::End();
	}

	void EditorLayer::DebugWindow()
	{
		ImGui::Begin("Debug Window");

		ImGui::Text("Focus: %s\nHover: %s\nDock: %s", m_ViewportFocused ? "Focused" : "No", m_ViewportHovered ? "Hovered" : "No", m_ViewportDocked ? "Docked" : "No");
		ImGui::Text("Debug mode: %s", m_DebugRenderer.DebugModeToString(s_DebugRendererMode).c_str());
		ImGui::Text("Viewport for EC: %s", m_EditorCamera.GetViewportStatusAsString().c_str());
		ImGui::Text("TitleBar: %s", Application::Get().IsTitleBarHovered() ? "1" : "0");

		ImGui::End();
	}

	void EditorLayer::RendererStatsWindow()
	{
		ImGui::Begin("Renderer Stats");

		ImGui::Text("Draw Calls: %i", Renderer2D::GetStats().DrawCalls);
		ImGui::Text("Quads: %i", Renderer2D::GetStats().QuadCount);
		ImGui::Text("Vertices: %i", Renderer2D::GetStats().GetTotalVertexCount());
		ImGui::Text("Indices: %i", Renderer2D::GetStats().GetTotalIndexCount());
		ImGui::Text("FPS: %i", static_cast<uint16_t>(ImGui::GetIO().Framerate));
		ImGui::Text("Frame time: %.3f ms", static_cast<uint16_t>(ImGui::GetIO().Framerate) / 1000.0f);

		std::string HoveredEntityName = m_HoveredEntity && m_HoveredEntity.HasComponent<TagComponent>() ? m_HoveredEntity.GetComponent<TagComponent>().Tag : "None";

		ImGui::Text("Hovered Entity: %s", HoveredEntityName.c_str());

		ImGui::End();
	}
}
