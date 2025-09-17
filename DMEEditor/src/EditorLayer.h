#pragma once

#include <dme.h>
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ContentBrowserPanel.h"
#include "Panels/PropertiesPanel.h"

#include "DME/Renderer/EditorCamera.h"
#include "DME/ImGui/ImGuiDMEEditor.h"


namespace DME
{
	class EditorLayer : public Layer
	{
	public: // Constructors and destructors

		EditorLayer();
		virtual ~EditorLayer() = default;

	public: // Layer overrides

		void OnAttach() override;
		void OnDetach() override;

		void OnUpdate(TimeStep ts) override;
		void OnImGuiRender() override;
		void OnDockspace() override;
		void OnEvent(Event& event) override;

	public: // Overlay render

		void OnOverlayRender();

	private: // SceneSerializer functions

		void NewScene();
		void OpenScene();
		void OpenScene(const std::filesystem::path& path);
		void SaveScene();
		void SaveSceneAs();

		void SerializeScene(Ref<Scene> scene, const std::filesystem::path& path);

	private: // Helper scene functions

		void OnDuplicateEntity();
		void DeleteSelectedEntity();

	private: // Scene state

		enum class SceneState
		{
			Edit = 0, Play = 1, Simulate = 2
		};

		SceneState m_SceneState = SceneState::Edit;

		void OnScenePlay();
		void OnSceneSimulate();
		void OnSceneStop();

	private: // UI functions

		void UIToolbar();
		void GizmosToolbar();
		void UITabBar();

	private: // Windows

		void ViewportWindow();
		void DebugWindow();
		void RendererStatsWindow();
		void ConsoleWindow();

	public: // Events

		bool OnKeyPressed(KeyPressedEvent& event);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);

	private: // Camera controller

		OrthographicCameraController m_CameraController;

	private: // Framebuffer & Scene

		Ref<Framebuffer> m_Framebuffer;
		Ref<Scene> m_ActiveScene;
		Ref<Scene> m_EditorScene;
		std::filesystem::path m_EditorScenePath;

	private: // Panels
		SceneHierarchyPanel m_SceneHierarchyPanel;
		ContentBrowserPanel m_ContentBrowser;
		PropertiesPanel m_PropertiesPanel;

	private: // Cameras

		EditorCamera m_EditorCamera;
		SceneCamera m_SceneCamera;

	private: // Entity

		Entity m_HoveredEntity;

	private: // Viewport

		bool m_BlockViewportEvents = false;
		bool m_PrimaryCamera = true;

		bool m_ViewportFocused = false, m_ViewportHovered = false, m_ViewportHoveredAndFocused = false, m_ViewportDocked = false;
		bool m_MainWindowUnDocked = false;
		bool m_MainWindowDocked = true;
		glm::vec2 m_ViewportBounds[2]{ };
		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };

	private: // Gizmos

		int m_GizmoType = -1;

	private: // Textures

		Ref<Texture2D> m_IconPlay, m_IconSimulate, m_IconStop;
		Ref<Texture2D> m_IconCursor, m_IconMove, m_IconRotate, m_IconScale;
		Ref<Texture2D> m_SettingButton;

	private: // Window variables

		bool m_ViewportWindow = true;
		bool m_SettingsWindow = true;
		bool m_PropertiesWindow = true;
		bool m_SceneHierarchyWindow = true;
		bool m_PropertiesPanelWindow = true;
		bool m_ContentBrowserWindow = true;

		bool m_DebugWindow = false;
		bool m_DemoWindow = false;
		bool m_RendererStatsWindow = false;
		bool m_ConsoleWindow = true;

	};

}
