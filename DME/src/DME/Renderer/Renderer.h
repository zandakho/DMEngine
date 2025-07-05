#pragma once

// --- DME/Renderer/ -----------------------------------------//
#include "DME/Renderer/Shader.h"								//
#include "DME/Renderer/RenderCommand.h"						//
#include "DME/Renderer/OrthographicCamera.h"					//
// -------------------------------------------------------------//

namespace DME
{

	class Renderer
	{
	public:
		
		static void Init();
		static void OnWindowResize(uint32_t width, uint32_t height);

		static void BeginScene(OrthographicCamera& camera);
		static void EndScene();

		static void Shutdown();

		static void Sumbit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));

		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

	private:

		struct SceneData
		{
			glm::mat4 ViewProjectionMatrix;
		};

		static Scope<SceneData> m_SceneData;

	};

}