#include "dmepch.h"

// --- source --------------------------------------------------//
#include "Sandbox2D.h"											//
// -------------------------------------------------------------//

// --- Dependencies --------------------------------------------//
#include <chrono>												//
#include <ImGui/imgui.h>										//
#include <glm/gtc/type_ptr.hpp>									//
#include <glm/gtc/matrix_transform.hpp>							//
// -------------------------------------------------------------//

namespace DME
{

	Sandbox2D::Sandbox2D() : Layer("Sandbox2D"), m_CameraController(1600.0f / 900.0f) { }


	void Sandbox2D::OnAttach() 
	{
		DME_PROFILE_FUNCTION();

		m_BlackFlagTexture = Texture2D::Create("C:/Engine/DME/Sandbox/assets/textures/Checkerboard.png");
	}

	void Sandbox2D::OnDetach()
	{
		DME_PROFILE_FUNCTION();

	}

	void Sandbox2D::OnUpdate(TimeStep ts)
	{

		DME_PROFILE_FUNCTION();
		
		m_CameraController.OnUpdate(ts);

		//Render
		Renderer2D::ResetStats();
		{
			DME_PROFILE_SCOPE("Renderer Prep");
			RenderCommand::SetClearColor({ 0.07f, 0.07f, 0.07f, 1.0f });
			RenderCommand::Clear();

		}

		{
			static float rotation = 0.0f;
			rotation += ts * 50.0f;

			DME_PROFILE_SCOPE("Renderer Draw");

			Renderer2D::BeginScene(m_CameraController.GetCamera());
			
			Renderer2D::DrawRotateQuad({ -1.0f, 0.0f, 0.5f }, { 0.8f, 0.8f }, glm::radians(45.0f), { 1.0f, 0.0f, 0.0f, 1.0f });
			Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, { 1.0f, 0.0f, 0.0f, 1.0f });
			Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 0.5f, 0.75f }, { 0.0f, 0.0f, 1.0f, 1.0f });
			Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1f }, { 20.0f, 20.0f }, m_BlackFlagTexture, 10.0f);
			Renderer2D::DrawRotateQuad({ -2.0f, 0.0f, 0.4f }, { 1.0f, 1.0f }, glm::radians(rotation), m_BlackFlagTexture, 20.0f);

			Renderer2D::EndScene();
			
			Renderer2D::BeginScene(m_CameraController.GetCamera());

			for (float y = -5.0f; y < 5.0f; y += 0.5f)
			{
				for (float x = -5.0f; x < 5.0f; x += 0.5f)
				{
					glm::vec4 color = { (x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.7f };
					Renderer2D::DrawQuad({ x, y, 0.2f }, { 0.45f, 0.45f }, { color });
				}

			}

			Renderer2D::EndScene();
		}

	}

	void Sandbox2D::OnImGuiRender()
	{
		DME_PROFILE_FUNCTION();

		ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		ImGui::DragFloat3("Camera Position", (float*)&m_CameraController.GetCameraPosition(), 0.1f, -FLT_MAX, FLT_MAX, "%.3f", ImGuiSliderFlags_ClampOnInput);
		ImGui::DragFloat("Camera Rotation", (float*)&m_CameraController.GetCameraRotation(), 1.0f, -FLT_MAX, FLT_MAX, "%.f", ImGuiSliderFlags_ClampOnInput);

		ImGui::NewLine();
		ImGui::Text(m_CameraController.GetZoomLevel() > 0.25f ? m_CameraController.GetZoomLevel() < 30.0f ? "Zoom Level: %.2f" : "Zoom Level: %.2f (min)" : "Zoom Level: %.2f (max)", m_CameraController.GetZoomLevel());

		ImGui::Text("Camera Position: X: %.1f Y: %.1f Z: %.1f",
			m_CameraController.GetCameraPosition().x,
			m_CameraController.GetCameraPosition().y,
			m_CameraController.GetCameraPosition().z);

		ImGui::Text("Renderer2D stats: ");
		ImGui::Text("Draw calls: %d", Renderer2D::GetStats().DrawCalls);

		ImGui::Text("Quad count: %d", Renderer2D::GetStats().QuadCount);
		ImGui::Text("Vertices count: %d", Renderer2D::GetStats().GetTotalVertexCount());
		ImGui::Text("Indices count: %d", Renderer2D::GetStats().GetTotalIndexCount());

		ImGui::End();
		
	}

	void Sandbox2D::OnEvent(Event& event)
	{
		m_CameraController.OnEvent(event);
	}
}

