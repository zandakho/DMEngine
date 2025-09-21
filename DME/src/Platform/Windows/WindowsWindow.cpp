#include "dmepch.h"

#include "DME/Events/ApplicationEvent.h"
#include "DME/Events/KeyEvent.h"
#include "DME/Events/MouseEvent.h"

#include "Platform/OpenGL/OpenGLContext.h"

#include "Platform/Windows/WindowsWindow.h"

#include "imgui.h"
#include "stb_image.h"

namespace DME
{
	static uint8_t s_GLFWWindowCount = 0;

	static void GLFWErrorCallback(int error, const char* description)
	{
		DME_CORE_ERROR("GLFWErrorCallback (Code: {0}), Hint: {1}", error, description)
	}

	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		DME_PROFILE_FUNCTION()

		Init(props);
	}

	WindowsWindow::~WindowsWindow()
	{
		DME_PROFILE_FUNCTION()

		Shutdown();
	}

	void WindowsWindow::OnUpdate()
	{
		DME_PROFILE_FUNCTION()

		glfwPollEvents();
		m_Context->SwapBuffers();
	}

	void WindowsWindow::SetVSync(bool enabled)
	{
		DME_PROFILE_FUNCTION()

		if (enabled)
		{
			glfwSwapInterval(1);
		}
		else glfwSwapInterval(0);

		m_Data.VSync = enabled;
	}

	bool WindowsWindow::IsVSync() const
	{
		return m_Data.VSync;
	}

	
	void WindowsWindow::Init(const WindowProps& props)
	{
		DME_PROFILE_FUNCTION()

		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;
		m_Data.VSync = true;


		DME_CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height)

		if (s_GLFWWindowCount == 0)
		{
			DME_PROFILE_SCOPE("glfwInit")
		
			int succes = glfwInit();
			DME_CORE_ASSERT(succes, "Could not initialize GLFW!")
			glfwSetErrorCallback(GLFWErrorCallback);
		}

		{
			DME_PROFILE_SCOPE("glfwCreateWindows")
			m_Window = glfwCreateWindow(static_cast<uint32_t>(props.Width), static_cast<uint32_t>(props.Height), m_Data.Title.c_str(), nullptr, nullptr);
			++s_GLFWWindowCount;
		}



		m_Context = GraphicsContext::Create(m_Window);
		m_Context->Init();

		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(true);

		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) 
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.Width = width;
			data.Height = height;

			WindowResizeEvent event(width, height);
			data.EventCallback(event);
		});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			WindowCloseEvent event;
			data.EventCallback(event);
		});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			
			switch (action)
			{

				case GLFW_PRESS:
				{
					KeyPressedEvent event(key, false);
					data.EventCallback(event);
					break;

				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent event(key);
					data.EventCallback(event);
					break;

				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent event(key, true);
					data.EventCallback(event);
					break;

				}
			}

		});

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, uint32_t keycode)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			KeyTypedEvent event(keycode);
			data.EventCallback(event);
		});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent event(button);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent event(button);
					data.EventCallback(event);
					break;
				}
			}
		});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xoffset, double yoffset)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseScrolledEvent event(static_cast<float>(xoffset), static_cast<float>(yoffset));
			data.EventCallback(event);
		});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xpos, double ypos)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			MouseMovedEvent event(static_cast<float>(xpos), static_cast<float>(ypos));
			data.EventCallback(event);
		});
	}

	void WindowsWindow::Shutdown()
	{
		DME_PROFILE_FUNCTION()

		glfwDestroyWindow(m_Window);
		--s_GLFWWindowCount;
		if (s_GLFWWindowCount == 0)
		{
			glfwTerminate();
		}
	}

}
