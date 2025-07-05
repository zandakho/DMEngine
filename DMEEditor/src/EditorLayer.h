#pragma once

#include <dme.h>

namespace DME
{
	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(TimeStep ts) override;
		virtual void OnImGuiRender() override;
		virtual void OnDockspace() override;
		virtual void OnEvent(Event& event) override;

	private:
		OrthographicCameraController m_CameraController;

	private:

		Ref<VertexArray> m_SquareVA;
		Ref<Shader> m_FlatColorShader;
		Ref<Framebuffer> m_Framebuffer;
		Ref<Scene> m_ActiveScene;

		Entity m_SquareEntity;
		Entity m_CameraEntity;
		Entity m_SecondCamera;


		bool m_PrimaryCamera = true;


		Ref<Texture2D> m_BlackFlagTexture;

		bool m_ViewportFocused = false, m_ViewportHovered = false;

		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };

		glm::vec4 m_SquareColor{ 1.0f, 0.0f, 0.0f, 1.0f };

	};

}
