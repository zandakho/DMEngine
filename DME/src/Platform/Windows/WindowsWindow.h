#pragma once

#include "DME/Core/Window.h"

#include "DME/Renderer/GraphicsContext.h"

#include <GLFW/glfw3.h>

namespace DME
{
	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		void OnUpdate() override;

		uint32_t GetWidth() const override { return m_Data.Width; }
		uint32_t GetHeight() const override { return m_Data.Height; }

		void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		void SetVSync(bool enabled) override;
		bool IsVSync() const override;

		virtual void* GetNativeWindow() const override { return m_Window; }
	private:

		virtual void Init(const WindowProps& props);
		virtual void Shutdown();

	private:
		GLFWwindow* m_Window;
		Scope<GraphicsContext> m_Context;

		struct WindowData
		{

			std::string Title = std::string();
			uint32_t Width = 1600, Height = 900;
			bool VSync = false;

			EventCallbackFn EventCallback;
		};

		WindowData m_Data;
	};
}
