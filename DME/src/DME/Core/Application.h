#pragma once

#include "DME/Core/Base.h"
#include "DME/Core/Window.h"
#include "DME/Core/TimeStep.h"
#include "DME/Core/LayerStack.h"

#include "DME/Events/Event.h"
#include "DME/Events/ApplicationEvent.h"

#include "DME/ImGui/ImGuiLayer.h"

int main(int argc, char** argv);

struct GLFWwindow;

namespace DME
{
	struct ApplicationCommandLineArgs
	{
		int Count = 0;
		char** Args = nullptr;

		const char* operator[](int index) const
		{
			DME_CORE_ASSERT(index < Count)
			return Args[index];
		}
	};

	struct ApplicationSpecification
	{
		std::string Name = "DMEEditor";
		std::string WorkingDirectory;
		ApplicationCommandLineArgs CommandLineArgs;

		std::string Icon = "Resources/Icons/TaskBar/TaskBar_TaskBarIcon_Img.png";

	};

	class Application
	{
	public:

		Application(const ApplicationSpecification& specification);
		virtual ~Application();
		
		void OnEvent(Event& event);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		Window& GetWindow() { return *m_Window; }

		GLFWwindow* GetNativeWindow() { return static_cast<GLFWwindow*>(Get().GetWindow().GetNativeWindow()); }

		void Shutdown();

		void Close() { m_Running = false; }
		void Minimize() { m_Minimized = true; }

		bool IsWindowMaximized() const;
		bool IsTitleBarHovered() const { return m_TitleBarHovered; }
		void IsTitleBarHovered(bool hovered) { m_TitleBarHovered = hovered; }

		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }

		static Application& Get() { return *s_Instance; }
		const ApplicationSpecification& GetSpecification() const { return m_Specification; }

	private:
		void Run();

		bool OnWindowClose(WindowCloseEvent& event);
		bool OnWindowResize(WindowResizeEvent& event);

	private:

		bool m_Running = true;
		bool m_Minimized = false;
		bool m_TitleBarHovered = false;

		Scope<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		LayerStack m_LayerStack;
		float m_LastFrameTime = 0.0f;
		ApplicationSpecification m_Specification;

	private:
		static Application* s_Instance;
		friend int ::main(int argc, char** argv);
	};

	Application* CreateApplication(ApplicationCommandLineArgs args);
}
