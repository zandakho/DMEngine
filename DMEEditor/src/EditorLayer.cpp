#include "dmepch.h"

#include "EditorLayer.h"

#include <chrono>
#include <ImGui/imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace DME
{

	EditorLayer::EditorLayer() : Layer("EditorLayer"), m_CameraController(1600.0f / 900.0f) { }


	void EditorLayer::OnAttach()
	{
		DME_PROFILE_FUNCTION(); 

		m_BlackFlagTexture = Texture2D::Create("C:/Engine/DME/Common/assets/textures/Checkerboard.png");

		FramebufferSpecification fbSpec;
		fbSpec.Width = 1600;
		fbSpec.Width = 900;
		m_Framebuffer = Framebuffer::Create(fbSpec);

		m_ActiveScene = CreateRef<Scene>();

		auto square = m_ActiveScene->CreateEntity("Square");
		square.AddComponent<SpriteRendererComponent>(glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f });
		
		m_SquareEntity = square;

		m_CameraEntity = m_ActiveScene->CreateEntity("Camera");
		m_CameraEntity.AddComponent<CameraComponent>();
		 
		m_SecondCamera = m_ActiveScene->CreateEntity("Second Camera");
		auto&& cc = m_SecondCamera.AddComponent<CameraComponent>();
		cc.Primary = false;


		class CameraController : public ScriptableEntity
		{
		public:

			void OnCreate()
			{
				auto& transform = GetComponent<TransformComponent>().Transform;
				transform[3][0] = rand() % 10 - 5.0f;
			}

			void OnDestroy()
			{

			}

			void OnUpdate(TimeStep ts)
			{

				auto& transform = GetComponent<TransformComponent>().Transform;
				float speed = 5.0f;

				if (Input::IsKeyPressed(Key::LeftControl) && Input::IsKeyPressed(Key::Left))
					transform[3][0] -= speed * ts;
				if (Input::IsKeyPressed(Key::LeftControl) && Input::IsKeyPressed(Key::Right))
					transform[3][0] += speed * ts;
				if (Input::IsKeyPressed(Key::LeftControl) && Input::IsKeyPressed(Key::Down))
					transform[3][1] -= speed * ts;
				if (Input::IsKeyPressed(Key::LeftControl) && Input::IsKeyPressed(Key::Up))
					transform[3][1] += speed * ts;
			}
		};
		
		m_CameraEntity.AddComponent<NativeScriptComponent>().Bind<CameraController>();
		m_SecondCamera.AddComponent<NativeScriptComponent>().Bind<CameraController>();

		m_SceneHierarchy.SetContext(m_ActiveScene);
	}

	void EditorLayer::OnDetach()
	{
		DME_PROFILE_FUNCTION();

	}

	void EditorLayer::OnUpdate(TimeStep ts)
	{
		DME_PROFILE_FUNCTION();

		if (FramebufferSpecification spec = m_Framebuffer->GetSpecification(); 
			m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && 
			(spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
		{
			m_Framebuffer->Resize(static_cast<uint32_t>(m_ViewportSize.x), static_cast<uint32_t>(m_ViewportSize.y));
			m_CameraController.OnResize(m_ViewportSize.x, m_ViewportSize.y);

			m_ActiveScene->OnViewportResize(static_cast<uint32_t>(m_ViewportSize.x), static_cast<uint32_t>(m_ViewportSize.y));

		}
		
		if(m_ViewportFocused)
			m_CameraController.OnUpdate(ts);

		//Render
		Renderer2D::ResetStats();
		m_Framebuffer->Bind();
		RenderCommand::SetClearColor({ 0.07f, 0.07f, 0.07f, 1.0f });
		RenderCommand::Clear();

		m_ActiveScene->OnUpdate(ts);

		m_Framebuffer->UnBind();

	}

	void EditorLayer::OnImGuiRender()
	{
		DME_PROFILE_FUNCTION();

		//static bool show = true;
		//ImGui::ShowDemoWindow(&show);

		EditorLayer::OnDockspace();

		m_SceneHierarchy.OnImGuiRender();

		ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_HorizontalScrollbar);

		ImGuiIO& io = ImGui::GetIO(); static_cast<void>(io);
		
		if (m_SquareEntity)
		{

			if (ImGui::CollapsingHeader("Scene"))
			{
				if (ImGui::Combo("Camera Select", &selected_items, items, 2))
				{
					selected_items == 0 ? m_PrimaryCamera = true : m_PrimaryCamera = false;
					m_CameraEntity.GetComponent<CameraComponent>().Primary = m_PrimaryCamera;
					m_SecondCamera.GetComponent<CameraComponent>().Primary = !m_PrimaryCamera;
				}

				{

					if (m_PrimaryCamera)
					{
						auto& camera1 = m_CameraEntity.GetComponent<CameraComponent>().Camera;
						float orthoSize = camera1.GetOrthographicSize();

						if (ImGui::DragFloat("Camera1", &orthoSize, 0.2f, 0.0f, FLT_MAX))
						{
							camera1.SetOrthographicSize(orthoSize);
						}

						
					}
					else
					{
						auto& camera2 = m_SecondCamera.GetComponent<CameraComponent>().Camera;
						float orthoSize = camera2.GetOrthographicSize();

						if (ImGui::DragFloat("Camera2", &orthoSize, 0.2f, 0.0f, FLT_MAX))
						{
							camera2.SetOrthographicSize(orthoSize);
						}
					}
					
				}

				m_PrimaryCamera ? 
					ImGui::DragFloat3("First Camera", glm::value_ptr(m_CameraEntity.GetComponent<TransformComponent>().Transform[3]), 0.1f, -FLT_MAX, FLT_MAX, "%.3f", ImGuiSliderFlags_ClampOnInput) 
					: 
					ImGui::DragFloat3("Second Camera", glm::value_ptr(m_SecondCamera.GetComponent<TransformComponent>().Transform[3]), 0.1f, -FLT_MAX, FLT_MAX, "%.3f", ImGuiSliderFlags_ClampOnInput);


				ImGui::Separator();
				auto& tag = m_SquareEntity.GetComponent<TagComponent>().Tag;
				ImGui::InputText("##name entities", (char*)tag.c_str(), 128);
				ImGui::Text("%s", tag.c_str());
				ImGui::Separator();

				auto& squareColor = m_SquareEntity.GetComponent<SpriteRendererComponent>().Color;
				ImGui::ColorEdit4("Square", (float*)&squareColor);

				if (ImGui::CollapsingHeader("Information"))
				{
					if (ImGui::TreeNode("Scene##information"))
					{

						ImGui::TreePop();
					} 

					if (ImGui::TreeNode("2D Render stats"))
					{
						ImGui::Text("Draw calls: %d", Renderer2D::GetStats().DrawCalls);

						ImGui::Text("Quad count: %d", Renderer2D::GetStats().QuadCount);
						ImGui::Text("Vertices count: %d", Renderer2D::GetStats().GetTotalVertexCount());
						ImGui::Text("Indices count: %d", Renderer2D::GetStats().GetTotalIndexCount());

						ImGui::TreePop();
					}
					
					if (ImGui::TreeNode("Render##information"))
					{
						ImGui::Text("FPS: %i", static_cast<uint32_t>(io.Framerate));
						ImGui::Text("Frame time: %.3f", static_cast<float>(io.Framerate) / 1000.f);
						ImGui::TreePop();
					} 
					
				}
			}

		}

		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(2.0f, 2.0f));

		ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();

		Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportFocused || !m_ViewportHovered);

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

		ImGui::Image(reinterpret_cast<void*>(static_cast<uintptr_t>(m_Framebuffer->GetColorAttachmentRendererID())), ImVec2(m_ViewportSize.x, m_ViewportSize.y), { 0, 1 }, { 1, 0 });

		ImGui::End();
		ImGui::PopStyleVar(1);

		
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
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Close")) Application::Get().Close();

				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		ImGui::End();
	}

	void EditorLayer::OnEvent(Event& event)
	{
		m_CameraController.OnEvent(event);
	}
}

