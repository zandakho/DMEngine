#include "dmepch.h"

#include "DME/Core/Application.h"

#include "DME/ImGui/ImGuiLayer.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "ImGuizmo.h"

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

		io.Fonts->AddFontFromFileTTF("assets/fonts/opensans/OpenSans-Bold.ttf", 15, nullptr, io.Fonts->GetGlyphRangesCyrillic());
		io.Fonts->AddFontFromFileTTF("assets/fonts/opensans/OpenSans-Bold.ttf", 18, nullptr, io.Fonts->GetGlyphRangesCyrillic());
		io.Fonts->AddFontFromFileTTF("assets/fonts/opensans/OpenSans-Bold.ttf", 21, nullptr, io.Fonts->GetGlyphRangesCyrillic());
		io.Fonts->AddFontFromFileTTF("assets/fonts/opensans/OpenSans-Regular.ttf", 15, nullptr, io.Fonts->GetGlyphRangesCyrillic());
		io.Fonts->AddFontFromFileTTF("assets/fonts/opensans/OpenSans-Regular.ttf", 18, nullptr, io.Fonts->GetGlyphRangesCyrillic());
		io.Fonts->AddFontFromFileTTF("assets/fonts/opensans/OpenSans-Regular.ttf", 21, nullptr, io.Fonts->GetGlyphRangesCyrillic());

		io.Fonts->AddFontFromFileTTF("assets/fonts/Roboto/Roboto.ttf", 15, nullptr, io.Fonts->GetGlyphRangesCyrillic());
		io.Fonts->AddFontFromFileTTF("assets/fonts/Roboto/Roboto.ttf", 18, nullptr, io.Fonts->GetGlyphRangesCyrillic());
		io.Fonts->AddFontFromFileTTF("assets/fonts/Roboto/Roboto.ttf", 21, nullptr, io.Fonts->GetGlyphRangesCyrillic());
		io.Fonts->AddFontFromFileTTF("assets/fonts/Roboto/Roboto-Italic.ttf", 15, nullptr, io.Fonts->GetGlyphRangesCyrillic());
		io.Fonts->AddFontFromFileTTF("assets/fonts/Roboto/Roboto-Italic.ttf", 18, nullptr, io.Fonts->GetGlyphRangesCyrillic());
		io.Fonts->AddFontFromFileTTF("assets/fonts/Roboto/Roboto-Italic.ttf", 21, nullptr, io.Fonts->GetGlyphRangesCyrillic());

		io.Fonts->AddFontFromFileTTF("assets/fonts/Noto/NotoSansDevanagari-Regular.ttf", 15, nullptr, io.Fonts->GetGlyphRangesCyrillic());
		io.Fonts->AddFontFromFileTTF("assets/fonts/Noto/NotoSansDevanagari-Regular.ttf", 18, nullptr, io.Fonts->GetGlyphRangesCyrillic());
		io.Fonts->AddFontFromFileTTF("assets/fonts/Noto/NotoSansDevanagari-Regular.ttf", 21, nullptr, io.Fonts->GetGlyphRangesCyrillic());

		io.Fonts->AddFontFromFileTTF("assets/fonts/Droid/DroidSansFallback.ttf", 15, nullptr, io.Fonts->GetGlyphRangesCyrillic());
		io.Fonts->AddFontFromFileTTF("assets/fonts/Droid/DroidSansFallback.ttf", 18, nullptr, io.Fonts->GetGlyphRangesCyrillic());
		io.Fonts->AddFontFromFileTTF("assets/fonts/Droid/DroidSansFallback.ttf", 21, nullptr, io.Fonts->GetGlyphRangesCyrillic());

		io.FontDefault = io.Fonts->AddFontFromFileTTF("assets/fonts/Roboto/Roboto.ttf", 18, nullptr, io.Fonts->GetGlyphRangesCyrillic());

		ImGuiStyle& style = ImGui::GetStyle();
		style.FrameRounding = 2.5f;
		style.PopupRounding = 2.5f;
		style.ScrollbarRounding = 2.5f;
		style.GrabRounding = 2.5f;

		style.WindowPadding = ImVec2(10.0f, 10.0f);
		style.FramePadding = ImVec2(10.0f, 5.0f);
		style.ItemSpacing = ImVec2(4.0f, 5.0f);
		style.ItemInnerSpacing = ImVec2(10.0f, 5.0f);
		style.TouchExtraPadding = ImVec2(0.0f, 0.0f);
		style.IndentSpacing = 25.0f;
		style.ScrollbarSize = 15.0f;
		style.GrabMinSize = 19.0f;

		style.WindowBorderSize = 0.0f;
		style.ChildBorderSize = 0.0f;
		style.PopupBorderSize = 0.0f;
		style.FrameBorderSize = 0.0f;

		style.TreeLinesRounding = 1.5f;
		style.TreeLinesSize = 1.5f;

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
		ImGuizmo::BeginFrame();
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

	void ImGuiLayer::SetImGuiStyle()
	{

	}
}