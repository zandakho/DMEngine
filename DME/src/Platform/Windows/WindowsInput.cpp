#include "dmepch.h"

// --- DME/Core/ ---------------------------------------------//
#include "DME/Core/Application.h"								//
#include "DME/Core/Input.h"									//
// -------------------------------------------------------------//

// --- Dependencies --------------------------------------------//
#include <GLFW/glfw3.h>											//
// -------------------------------------------------------------//

namespace DME
{

	bool Input::IsKeyPressed(KeyCode keycode)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetKey(window, static_cast<uint32_t>(keycode));
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool Input::IsMouseButtonPressed(MouseCode button)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetMouseButton(window, static_cast<uint32_t>(button));
		return state == GLFW_PRESS;
	}

	std::pair<float, float> Input::GetMousePosition()
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		double x, y;
		glfwGetCursorPos(window, &x, &y);

		return { static_cast<float>(x), static_cast<float>(y) };
	}

	float Input::GetMouseX()
	{
		auto [x, y] = GetMousePosition();
		return static_cast<float>(x);
	}

	float Input::GetMouseY()
	{
		auto [x, y] = GetMousePosition();
		return static_cast<float>(y);
	}

}