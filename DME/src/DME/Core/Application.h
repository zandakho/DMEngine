#pragma once

#include "DME/Core/Base.h"
#include "DME/Core/Window.h"
#include "DME/Core/TimeStep.h"
#include "DME/Core/LayerStack.h"

#include "DME/Events/Event.h"
#include "DME/Events/ApplicationEvent.h"

#include "DME/ImGui/ImGuiLayer.h"

int main(int argc, char** argv);

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
		std::string Name = "DME Application";
		std::string WorkingDirectory;
		ApplicationCommandLineArgs CommandLineArgs;
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

		void Shutdown();

		void Close() { m_Running = false; }
		void Minimize() { m_Minimized = true; }
		void Fullscreen() { m_Fullscreen = true; }
		void NoFullscreen() { m_Fullscreen = false; }

		bool IsWindowMaximized(bool maximized);

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
		bool m_Fullscreen = false;
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
