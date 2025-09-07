#pragma once

#include <dme.h>
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ContentBrowserPanel.h"

#include "DME/Renderer/EditorCamera.h"
#include "DME/ImGui/ImGuiDMEEditor.h"

namespace DME
{
	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		void OnUpdate(TimeStep ts) override;
		virtual void OnImGuiRender() override;
		void OnDockspace() override;
		void OnEvent(Event& event) override;
		void OnOverlayRender();

	private:

		void NewScene();
		void OpenScene();
		void OpenScene(const std::filesystem::path& path);
		void SaveScene();
		void SaveSceneAs();

		void SerializeScene(Ref<Scene> scene, const std::filesystem::path& path);

		void OnDuplicateEntity();

		void OnScenePlay();
		void OnSceneSimulate();
		void OnSceneStop();

		void UIToolbar();
		void GizmosToolbar();
		void UITabBar();
		
		void ViewportWindow();

	private:
		bool OnKeyPressed(KeyPressedEvent& event);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);

	private:
		OrthographicCameraController m_CameraController;

	private:
		Ref<VertexArray> m_SquareVA;
		Ref<Framebuffer> m_Framebuffer;
		Ref<Shader> m_FlatColorShader;
		Ref<Scene> m_ActiveScene;
		Ref<Scene> m_EditorScene;
		std::filesystem::path m_EditorScenePath;
		
	private:
		SceneHierarchyPanel m_SceneHierarchy;
		ContentBrowserPanel m_ContentBrowser;
		EditorCamera m_EditorCamera;

	private:

		Entity m_HoveredEntity;
		
		bool m_BlockViewportEvents = false;
		bool m_PrimaryCamera = true;

		int m_GizmoType = -1;

		enum class SceneState
		{
			Edit = 0, Play = 1, Simulate = 2
		};

		SceneState m_SceneState = SceneState::Edit;

		bool m_ViewportFocused = false, m_ViewportHovered = false, m_ViewportHoveredAndFocused, m_ViewportDocked = false;

		glm::vec2 m_ViewportBounds[2]{ };
		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
		
	private: // Textures
		
		Ref<Texture2D> m_IconPlay, m_IconSimulate, m_IconStop;

	private: // Window variables

		bool m_ViewportWindow;
		bool m_SettingsWindow;
		bool m_PropertiesWindow;
		bool m_SceneHierarchyWindow;
		bool m_ContentBrowserWindow;

		bool m_DebugWindow;
		bool m_DemoWindow;
	
	};

}
