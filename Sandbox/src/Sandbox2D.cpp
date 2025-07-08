#include "dmepch.h"

#include "Sandbox2D.h"

#include <chrono>
#include <ImGui/imgui.h>
#include <glm/gtc/type_ptr.hpp>

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

	}

	void Sandbox2D::OnImGuiRender()
	{
		DME_PROFILE_FUNCTION();

		ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		ImGui::NewLine();
		ImGui::Text(m_CameraController.GetZoomLevel() > 0.25f ? m_CameraController.GetZoomLevel() < 30.0f ? "Zoom Level: %.2f" : "Zoom Level: %.2f (min)" : "Zoom Level: %.2f (max)", m_CameraController.GetZoomLevel());

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

