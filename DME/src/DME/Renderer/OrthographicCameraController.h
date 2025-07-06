#pragma once

#include "DME/Core/TimeStep.h"

#include "DME/Events/ApplicationEvent.h"
#include "DME/Events/MouseEvent.h"

#include "DME/Renderer/OrthographicCamera.h"

namespace DME
{

	class OrthographicCameraController
	{
	public:
		OrthographicCameraController(float aspectRatio, bool rotation = true);

	public:
		void OnUpdate(TimeStep ts);
		void OnEvent(Event& event);

		void OnResize(float width, float height);

		OrthographicCamera& GetCamera() { return m_Camera; }
		const OrthographicCamera& GetCamera() const { return m_Camera; }

		void SetZoomLevel(float level) { m_ZoomLevel = level; CalculateView(); }
		float& GetZoomLevel() { return m_ZoomLevel; }
		float& GetCameraRotation() { return m_CameraRotation; }

		void SetCameraStart(float x, float y, float z) { m_CameraPosition = { x, y, z }; }

		glm::vec3& GetCameraPosition() { return m_CameraPosition; }
				
	private:

		void CalculateView();

		bool OnMouseScrolled(MouseScrolledEvent& event);
		bool OnWindowResized(WindowResizeEvent& event);

	private:
		OrthographicCamera m_Camera;

	private:

		float m_AspectRatio;
		float m_ZoomLevel { 1.0f };
		float m_CameraTranslationSpeed { 5.0f };
		float m_CameraRotationSpeed { 100.0f };
		float m_CameraRotation { 0.0f };

		bool m_Rotation = false;

		glm::vec3 m_CameraPosition { 0.0f, 0.0f, 0.0f };
	};
}