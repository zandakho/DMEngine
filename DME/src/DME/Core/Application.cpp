#include "dmepch.h"

// --- DME/Core/ ---------------------------------------------//
#include "DME/Core/Log.h"							            //
#include "DME/Core/Input.h"						            //
#include "DME/Core/Application.h"					            //
// -------------------------------------------------------------//

// --- DME/Renderer/ -----------------------------------------//
#include "DME/Renderer/Renderer.h"				            //
// -------------------------------------------------------------//
													   
// --- Dependencies --------------------------------------------//
#include <GLFW/glfw3.h>								            //
#include <print>
// -------------------------------------------------------------//

namespace DME
{

	Application* Application::s_Instance = nullptr;

	Application::Application(const std::string& name)
	{
		HZ_PROFILE_FUNCTION();

		DME_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		m_Window = Window::Create(WindowProps(name));
		m_Window->SetEventCallback(DME_BIND_EVENT_FN(Application::OnEvent));

		Renderer::Init();

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
	}

	Application::~Application()
	{
		HZ_PROFILE_FUNCTION();

		Renderer::Shutdown();
	}

	void Application::PushLayer(Layer* layer)
	{
		HZ_PROFILE_FUNCTION();
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* overlay)
	{
		HZ_PROFILE_FUNCTION();

		m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

	void Application::OnEvent(Event& event)
	{
		HZ_PROFILE_FUNCTION();
		
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<WindowCloseEvent>(DME_BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(DME_BIND_EVENT_FN(Application::OnWindowResize));

		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
		{
			if (event.m_Handled)
				break;
			(*it)->OnEvent(event);
		}
	}

	void Application::Run()
	{
		HZ_PROFILE_FUNCTION();

		while (m_Running)
		{
			HZ_PROFILE_SCOPE("RunLoop");
		
			float time = static_cast<float>(glfwGetTime());
			TimeStep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			if (!m_Minimized)
			{
				{
					
					HZ_PROFILE_SCOPE("LayerStack OnUpdate")

					for (Layer* layer : m_LayerStack)
						layer->OnUpdate(timestep);

				}
				
				m_ImGuiLayer->Begin();
				{
					HZ_PROFILE_SCOPE("LayerStack OnImGuiRender")
					for (Layer* layer : m_LayerStack)
						layer->OnImGuiRender();
				}
				m_ImGuiLayer->End();
			} 

			m_Window->OnUpdate();

		}
	}

	void Application::Shutdown()
	{
		HZ_PROFILE_FUNCTION();
		delete s_Instance;
	}

	bool Application::OnWindowClose(WindowCloseEvent& event)
	{
		m_Running = false;
		return true;
	} 

	bool Application::OnWindowResize(WindowResizeEvent& event)
	{
		HZ_PROFILE_FUNCTION();

		if (event.GetWidth() == 0 || event.GetHeight() == 0)
		{
			m_Minimized = true; 
			return false;
		}

		m_Minimized = false;
		Renderer::OnWindowResize(static_cast<uint32_t>(event.GetWidth()), static_cast<uint32_t>(event.GetHeight()));
		return false;
	}

}