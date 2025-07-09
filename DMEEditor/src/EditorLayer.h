#pragma once

#include <dme.h>
#include "Panels/SceneHierarchyPanel.h"


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
		void SaveSceneAs();

	private:
		bool OnKeyPressed(KeyPressedEvent& event);

	private:
		OrthographicCameraController m_CameraController;

	private:
		Ref<VertexArray> m_SquareVA;
		Ref<Shader> m_FlatColorShader;
		Ref<Framebuffer> m_Framebuffer;
		Ref<Scene> m_ActiveScene;

	private:
		SceneHierarchyPanel m_SceneHierarchy;

	private:

		
		bool m_PrimaryCamera = true;

		int m_GizmoType = -1;

		bool m_ViewportFocused = false, m_ViewportHovered = false;

		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };

		glm::vec4 m_SquareColor{ 1.0f, 0.0f, 0.0f, 1.0f };


	};

}
