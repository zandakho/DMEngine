#include "dmepch.h"

#include "DME/Scene/SceneCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace DME
{

	SceneCamera::SceneCamera()
	{
		RecalculateProjection();
		m_CameraTexture = Texture2D::Create("Resources/Icons/Viewport/ViewportCamera_Img.png");
	}

	void SceneCamera::SetPerspective(float verticalFOV, float nearClip, float farClip)
	{
		m_ProjectionType = ProjectionType::Perspective;
		m_PerspectiveVerticalFOV = verticalFOV;
		m_PerspectiveNear = nearClip;
		m_PerspectiveFar = farClip;

		RecalculateProjection();
	}

	void SceneCamera::SetOrthographic(float size, float nearClip, float farClip)
	{
		m_ProjectionType = ProjectionType::Orthographic;
		m_OrthographicSize = size;
		m_OrthographicNear = nearClip;
		m_OrthographicFar = farClip;
	
		RecalculateProjection();
	}
	

	void SceneCamera::SetViewportSize(uint32_t width, uint32_t height)
	{
		DME_CORE_ASSERT(width > 0 && height > 0);
		m_AspectRatio = static_cast<float>(width) / static_cast<float>(height);
		RecalculateProjection();
	}

	void SceneCamera::RecalculateProjection()
	{
		if (m_ProjectionType == ProjectionType::Perspective)
		{
			m_Projection = glm::perspective(m_PerspectiveVerticalFOV, m_AspectRatio, m_PerspectiveNear, m_PerspectiveFar);
		}

		if (m_ProjectionType == ProjectionType::Orthographic)
		{
			float orthographicLeft = -m_OrthographicSize * m_AspectRatio * 0.5f;
			float orthographicRight = m_OrthographicSize * m_AspectRatio * 0.5f;
			float orthographicBottom = -m_OrthographicSize * 0.5f;
			float orthographicTop = m_OrthographicSize * 0.5f;
			m_Projection = glm::ortho(orthographicLeft, orthographicRight, orthographicBottom, orthographicTop, m_OrthographicNear, m_OrthographicFar);
		}
		

	}

}