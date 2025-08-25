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
			DME_CORE_ASSERT(index < Count);
			return Args[index];
		}
	};

	class Application
	{
	public:

		Application(const std::string& name = "DME App", ApplicationCommandLineArgs args = ApplicationCommandLineArgs());
		virtual ~Application();
		
		void OnEvent(Event& event);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		Window& GetWindow() { return *m_Window; }

		void Shutdown();

		void Close() { m_Running = false; }

		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }

		static Application& Get() { return *s_Instance; }
		ApplicationCommandLineArgs GetCommandLineArgs() const { return m_CommandLineArgs; }

	private:
		void Run();

		bool OnWindowClose(WindowCloseEvent& event);
		bool OnWindowResize(WindowResizeEvent& event);

	private:

		bool m_Running = true;
		bool m_Minimized = false;
		Scope<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		LayerStack m_LayerStack;
		float m_LastFrameTime = 0.0f;
		ApplicationCommandLineArgs m_CommandLineArgs;




	private:
		static Application* s_Instance;
		friend int ::main(int argc, char** argv);
	};

	Application* CreateApplication(ApplicationCommandLineArgs args);
}
