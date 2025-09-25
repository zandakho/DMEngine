#include "dmepch.h"

#include "DME/Core/Application.h"
#include "DME/Core/Input.h"

#include <GLFW/glfw3.h>

namespace DME
{

	bool Input::IsKeyPressed(KeyCode keycode)
	{
		auto window = Application::Get().GetNativeWindow();
		auto state = glfwGetKey(window, static_cast<uint32_t>(keycode));
		return state == GLFW_PRESS;
	}

	bool Input::IsMouseButtonPressed(MouseCode button)
	{
		auto window = Application::Get().GetNativeWindow();
		auto state = glfwGetMouseButton(window, static_cast<uint32_t>(button));
		return state == GLFW_PRESS;
	}

	bool Input::IsMouseButtonReleased(MouseCode button)
	{
		auto window = Application::Get().GetNativeWindow();
		auto state = glfwGetMouseButton(window, static_cast<uint32_t>(button));
		return state == GLFW_REPEAT;
	}

	std::pair<float, float> Input::GetMousePosition()
	{
		auto window = Application::Get().GetNativeWindow();
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

	bool DME::Input::IsKeyPressedMode(KeyCode keycode)
	{
		auto window = Application::Get().GetNativeWindow();
		auto state = glfwGetKey(window, static_cast<uint32_t>(keycode));
		return state == GLFW_PRESS;
	}

	bool DME::Input::IsKeyReleasedMode(KeyCode keycode)
	{
		auto window = Application::Get().GetNativeWindow();
		auto state = glfwGetKey(window, static_cast<uint32_t>(keycode));
		return state == GLFW_REPEAT;
	}

}
