#pragma once

#include <dme.h>
#include <ImGui/imgui.h>
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/ContentBrowserPanel.h"

#include "DME/Renderer/EditorCamera.h"


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

	private:

		void NewScene();
		void OpenScene();
		void OpenScene(const std::filesystem::path& path);
		void SaveSceneAs();

		void OnScenePlay();
		void OnSceneStop();

		void UIToolbar();
		void GizmosToolbar();
		

	private:
		bool OnKeyPressed(KeyPressedEvent& event);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);

	private:
		OrthographicCameraController m_CameraController;

	private:
		Ref<VertexArray> m_SquareVA;
		Ref<Shader> m_FlatColorShader;
		Ref<Framebuffer> m_Framebuffer;
		Ref<Scene> m_ActiveScene;
		
	private:
		SceneHierarchyPanel m_SceneHierarchy;
		ContentBrowserPanel m_ContentBrowser;
		EditorCamera m_EditorCamera;

	private:

		Entity m_HoveredEntity;
		
		bool m_BlockViewportEvents = false;
		bool m_PrimaryCamera = true;
		bool m_DemoWindow = false;

		int m_GizmoType = -1;

		enum class SceneState
		{
			Edit = 0, Play = 1
		};

		SceneState m_SceneState = SceneState::Edit;

		bool m_ViewportFocused = false, m_ViewportHovered = false, m_ViewportDocked = false;

		glm::vec2 m_ViewportBounds[2];

		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };

		glm::vec4 m_SquareColor{ 1.0f, 0.0f, 0.0f, 1.0f };

		Ref<Texture2D> m_IconPlay, m_IconStop;

	};

}
