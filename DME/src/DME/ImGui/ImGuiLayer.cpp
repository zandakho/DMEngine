#include "dmepch.h"

// --- DME/Core/ ---------------------------------------------//
#include "DME/Core/Application.h"								//
// -------------------------------------------------------------//

// --- DME/ImGui/ --------------------------------------------//
#include "DME/ImGui/ImGuiLayer.h"								//
// -------------------------------------------------------------//

// --- Dependencies --------------------------------------------//
#include "imgui.h"												//
#include "backends/imgui_impl_glfw.h"							//
#include "backends/imgui_impl_opengl3.h"						//
#include <GLFW/glfw3.h>											//
#include <glad/glad.h>											//
// -------------------------------------------------------------//



namespace DME
{

	ImGuiLayer::ImGuiLayer() : Layer("ImGuiLayer") {}

	void ImGuiLayer::OnAttach()
	{
		DME_PROFILE_FUNCTION();

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); static_cast<void>(io);
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

		float fontSize = 18.0f; 
		io.Fonts->AddFontFromFileTTF("C:/Engine/DME/Common/assets/fonts/opensans/OpenSans-Bold.ttf", fontSize);
		io.Fonts->AddFontFromFileTTF("C:/Engine/DME/Common/assets/fonts/opensans/OpenSans-Regular.ttf", fontSize);

		io.Fonts->AddFontFromFileTTF("C:/Engine/DME/Common/assets/fonts/Roboto/Roboto.ttf", fontSize);
		io.Fonts->AddFontFromFileTTF("C:/Engine/DME/Common/assets/fonts/Roboto/Roboto-Italic.ttf", fontSize);

		io.Fonts->AddFontFromFileTTF("C:/Engine/DME/Common/assets/fonts/Noto/NotoSansDevanagari-Regular.ttf", fontSize);

		io.Fonts->AddFontFromFileTTF("C:/Engine/DME/Common/assets/fonts/Droid/DroidSansFallback.ttf", fontSize);

		io.FontDefault = io.Fonts->AddFontFromFileTTF("C:/Engine/DME/Common/assets/fonts/Roboto/Roboto.ttf", fontSize);

		ImGuiStyle& style = ImGui::GetStyle();

		style.FrameRounding = 2.5f;
		style.PopupRounding = 2.5f;
		style.ScrollbarRounding = 2.5f;
		style.GrabRounding = 2.5f;

		style.WindowPadding = ImVec2(8.0f, 8.0f);
		style.FramePadding = ImVec2(5.0f, 4.0f);
		style.ItemSpacing = ImVec2(4.0f, 4.0f);
		style.ItemInnerSpacing = ImVec2(5.0f, 5.0f);
		style.TouchExtraPadding = ImVec2(0.0f, 0.0f);
		style.IndentSpacing = 21.0f;
		style.ScrollbarSize = 15.0f;
		style.GrabMinSize = 19.0f;

		style.WindowBorderSize = 0.0f;
		style.ChildBorderSize = 0.0f;
		style.PopupBorderSize = 0.0f;
		style.FrameBorderSize = 0.0f;

		Application& app = Application::Get();
		GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 410");
	}

	void ImGuiLayer::OnDetach()
	{
		DME_PROFILE_FUNCTION();
		
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::OnEvent(Event& event)
	{
		ImGuiIO& io = ImGui::GetIO(); static_cast<void>(io);
		if (m_BlockEvents)
		{
			event.m_Handled |= event.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;
			event.m_Handled |= event.IsInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;
		}
	}

	void ImGuiLayer::Begin()
	{
		DME_PROFILE_FUNCTION();
		
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiLayer::End()
	{
		DME_PROFILE_FUNCTION();
		
		ImGuiIO& io = ImGui::GetIO(); static_cast<void>(io);
		Application& app = Application::Get();

		io.DisplaySize = ImVec2(static_cast<float>(app.GetWindow().GetWidth()), static_cast<float>(app.GetWindow().GetHeight()));
		ImGui::Render();

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}
}