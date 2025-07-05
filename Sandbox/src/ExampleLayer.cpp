#include "ExampleLayer.h"

#include "ImGui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Platform/OpenGL/OpenGLShader.h>

namespace DME
{

	ExampleLayer::ExampleLayer()
		: Layer("Example"), m_CameraController(1600.0f / 900.0f, true)
	{
		// First /////////////////////////////////////////////
		m_VertexArray = VertexArray::Create();

		float vertices[3 * 7] =
		{
			/*   Position  */		 /*      Color       */
		   -0.5f, -0.5f, 0.0f,		 1.0f, 0.0f, 0.0f, 1.0f,
			0.5f, -0.5f, 0.0f,		 0.0f, 1.0f, 0.0f, 1.0f,
			0.0f,  0.5f, 0.0f,		 0.0f, 0.0f, 1.0f, 1.0f

		};

		Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(vertices, sizeof(vertices));
		BufferLayout layout = {
			{ShaderDataType::Float3, "a_Position"},
			{ShaderDataType::Float4, "a_Color"}
		};

		vertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		uint32_t indices[3] = { 0, 1, 2 };
		Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));
		m_VertexArray->SetIndexBuffer(indexBuffer);
		// End First //////////////////////////////////////////

		// Second /////////////////////////////////////////////

		m_SquareVA = VertexArray::Create();

		float squarevertices[5 * 4] =
		{
			/*   Position  */
		   -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
		   -0.5f,  0.5f, 0.0f, 0.0f, 1.0f

		};

		Ref<VertexBuffer> squareVB = VertexBuffer::Create(squarevertices, sizeof(squarevertices));
		squareVB->SetLayout({
			{ShaderDataType::Float3, "a_Position"},
			{ShaderDataType::Float2, "a_TextCoord"}
			});
		m_SquareVA->AddVertexBuffer(squareVB);

		uint32_t squareindices[6] = { 0, 1, 2, 2, 3, 0 };
		Ref<IndexBuffer> squareIB = IndexBuffer::Create(squareindices, sizeof(squareindices) / sizeof(uint32_t));
		m_SquareVA->SetIndexBuffer(squareIB);

		// End Second /////////////////////////////////////////

		// First shader ///////////////////////////////////////
		std::string vertexSrc = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;
			
			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;


			out vec3 v_Position;
			out vec4 v_Color;

			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
			}
		
		)";

		std::string fragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;

			in vec3 v_Position;
			in vec4 v_Color;

			void main()
			{
				color = vec4(v_Position * 0.5 + 0.2, 1.0);
				color = v_Color;
			}
		
		)";

		m_Shader = Shader::Create("VertexPosColor", vertexSrc, fragmentSrc);
		// End First shader ///////////////////////////////////

		// Second shader //////////////////////////////////////

		std::string flatColorShaderVertexSrc = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;

			void main()
			{
				v_Position = a_Position;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
			}
		
		)";

		std::string flatColorShaderFragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;

			in vec3 v_Position;

			uniform vec4 u_Color;
			
			uniform vec3 u_LightDir;

			void main()
			{
				color = u_Color;
			}
		
		)";

		m_BlueShader = Shader::Create("FlatColor", flatColorShaderVertexSrc, flatColorShaderFragmentSrc);

		// End second shader //////////////////////////////////

		auto textureShader = m_ShaderLibrary.Load("C:/Engine/DME/Sandbox/assets/shaders/Texture.glsl");

		m_ChernoLogoTexture = Texture2D::Create("C:/Engine/DME/Sandbox/assets/textures/FlagUSA_AnimeGirl.png");

		std::dynamic_pointer_cast<OpenGLShader>(textureShader)->Bind();
		std::dynamic_pointer_cast<OpenGLShader>(textureShader)->UploadUniformInt("u_Texture", 0);

	}

	void ExampleLayer::OnUpdate(TimeStep ts)
	{

		m_CameraController.OnUpdate(ts);

		RenderCommand::SetClearColor({ 0.07f, 0.07f, 0.07f, 1.0f });
		RenderCommand::Clear();

		Renderer::BeginScene(m_CameraController.GetCamera());

		glm::mat4 scale = glm::scale(glm::mat4(0.5f), glm::vec3(0.1f));

		std::dynamic_pointer_cast<OpenGLShader>(m_BlueShader)->Bind();
		std::dynamic_pointer_cast<OpenGLShader>(m_BlueShader)->UploadUniformFloat4("u_Color", QuadColor);

		for (int x = 0; x < square_count[0]; x++)
		{
			for (int y = 0; y < square_count[1]; y++)
			{
				glm::vec3 pos((x + 9) * 0.11f, (y + 9) * 0.11f, 0.0f);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
				Renderer::Sumbit(m_BlueShader, m_SquareVA, transform);
			}
		}

		auto textureShader = m_ShaderLibrary.Get("Texture");

		m_ChernoLogoTexture->Bind();
		Renderer::Sumbit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

		// Triangle
		// Renderer::Sumbit(m_Shader, m_VertexArray);

		Renderer::EndScene();
	}

	void ExampleLayer::OnImGuiRender()
	{

	}

	void ExampleLayer::OnEvent(Event& event)
	{
		m_CameraController.OnEvent(event);
	}

}