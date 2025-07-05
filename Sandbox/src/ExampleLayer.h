#pragma once

#include "dme.h"

namespace DME
{

	class ExampleLayer : public Layer
	{
	public:
		ExampleLayer();

		void OnUpdate(TimeStep ts) override;
		virtual void OnImGuiRender() override;
		void OnEvent(Event& event) override;

	private:
		ShaderLibrary m_ShaderLibrary;

		Ref<Shader> m_BlueShader;
		Ref<VertexArray> m_SquareVA;

		Ref<Shader> m_Shader;
		Ref<VertexArray> m_VertexArray;

		Ref<Texture2D> m_ChernoLogoTexture;

		OrthographicCameraController m_CameraController;

		glm::vec4 QuadColor{ 0.0f, 0.0f, 0.7f, 1.0f };

		int square_count[2]{ 9, 9 };

	};
}
