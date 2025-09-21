#include "dmepch.h"

#include "DME/Core/Application.h"

#include "DME/ImGui/ImGuiLayer.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>

namespace DME
{

	ImGuiLayer::ImGuiLayer() : Layer("ImGuiLayer") {}

	void ImGuiLayer::OnAttach()
	{
		DME_PROFILE_FUNCTION()

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); static_cast<void>(io);
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		io.ConfigFlags &= ~ImGuiConfigFlags_NoMouseCursorChange;
		io.ConfigViewportsNoAutoMerge = true;
		io.ConfigViewportPlatformFocusSetsImGuiFocus = true;

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

		ImGui::GetStyle().Colors[ImGuiCol_Text]								= ImColor(220, 220, 220, 255);
		ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]						= ImColor(128, 128, 128, 255);
		ImGui::GetStyle().Colors[ImGuiCol_WindowBg]							= ImColor(26, 26, 26, 255);
		ImGui::GetStyle().Colors[ImGuiCol_ChildBg]							= ImColor(20, 20, 20, 255);
		ImGui::GetStyle().Colors[ImGuiCol_PopupBg]							= ImColor(56, 56, 56, 255);
		ImGui::GetStyle().Colors[ImGuiCol_Border]							= ImColor(20, 20, 20, 255);
		ImGui::GetStyle().Colors[ImGuiCol_BorderShadow]						= ImColor(0, 0, 0, 0);
		ImGui::GetStyle().Colors[ImGuiCol_FrameBg]							= ImColor(15, 15, 15, 255);
		ImGui::GetStyle().Colors[ImGuiCol_FrameBgHovered]					= ImColor(35, 35, 35, 255);
		ImGui::GetStyle().Colors[ImGuiCol_FrameBgActive]					= ImColor(25, 25, 25, 255);
		ImGui::GetStyle().Colors[ImGuiCol_TitleBg]							= ImColor(12, 12, 12, 255);
		ImGui::GetStyle().Colors[ImGuiCol_TitleBgActive]					= ImColor(12, 12, 12, 255);
		ImGui::GetStyle().Colors[ImGuiCol_TitleBgCollapsed]					= ImColor(12, 12, 12, 255);
		ImGui::GetStyle().Colors[ImGuiCol_MenuBarBg]						= ImColor(20, 20, 20, 255);
		ImGui::GetStyle().Colors[ImGuiCol_ScrollbarBg]						= ImColor(0, 0, 0, 0);
		ImGui::GetStyle().Colors[ImGuiCol_ScrollbarGrab]					= ImColor(40, 40, 40, 255);
		ImGui::GetStyle().Colors[ImGuiCol_ScrollbarGrabHovered]				= ImColor(55, 55, 55, 255);
		ImGui::GetStyle().Colors[ImGuiCol_ScrollbarGrabActive]				= ImColor(70, 70, 70, 255);
		ImGui::GetStyle().Colors[ImGuiCol_CheckMark]						= ImColor(125, 125, 125, 255);
		ImGui::GetStyle().Colors[ImGuiCol_SliderGrab]						= ImColor(125, 125, 125, 255);
		ImGui::GetStyle().Colors[ImGuiCol_SliderGrabActive]					= ImColor(120, 120, 120, 255);
		ImGui::GetStyle().Colors[ImGuiCol_Button]							= ImColor(50, 50, 50, 255);
		ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered]					= ImColor(55, 55, 55, 255);
		ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]						= ImColor(45, 45, 45, 255);
		ImGui::GetStyle().Colors[ImGuiCol_Header]							= ImColor(47, 47, 47, 255);
		ImGui::GetStyle().Colors[ImGuiCol_HeaderHovered]					= ImColor(47, 47, 47, 255);
		ImGui::GetStyle().Colors[ImGuiCol_HeaderActive]						= ImColor(47, 47, 47, 255);
		ImGui::GetStyle().Colors[ImGuiCol_Separator]						= ImColor(30, 30, 30, 255);
		ImGui::GetStyle().Colors[ImGuiCol_SeparatorHovered]					= ImColor(35, 35, 35, 255);
		ImGui::GetStyle().Colors[ImGuiCol_SeparatorActive]					= ImColor(25, 25, 25, 255);
		ImGui::GetStyle().Colors[ImGuiCol_ResizeGrip]						= ImColor(0, 0, 0, 0);
		ImGui::GetStyle().Colors[ImGuiCol_ResizeGripHovered]				= ImColor(20, 20, 20, 255);
		ImGui::GetStyle().Colors[ImGuiCol_ResizeGripActive]					= ImColor(15, 15, 15, 255);
		ImGui::GetStyle().Colors[ImGuiCol_InputTextCursor]					= ImColor(220, 220, 220, 255);
		ImGui::GetStyle().Colors[ImGuiCol_Tab]								= ImColor(21, 21, 21, 255);
		ImGui::GetStyle().Colors[ImGuiCol_TabHovered]						= ImColor(21, 21, 21, 255);
		ImGui::GetStyle().Colors[ImGuiCol_TabSelected]						= ImColor(24, 24, 24, 255);
		ImGui::GetStyle().Colors[ImGuiCol_TabSelectedOverline]				= ImColor(0, 0, 0, 0);
		ImGui::GetStyle().Colors[ImGuiCol_TabDimmed]						= ImColor(21, 21, 21, 255);
		ImGui::GetStyle().Colors[ImGuiCol_TabDimmedSelected]				= ImColor(24, 24, 24, 255);
		ImGui::GetStyle().Colors[ImGuiCol_TabDimmedSelectedOverline]		= ImColor(0, 0, 0, 0);
		ImGui::GetStyle().Colors[ImGuiCol_DockingPreview]					= ImColor(20, 20, 20, 255);
		ImGui::GetStyle().Colors[ImGuiCol_DockingEmptyBg]					= ImColor(5, 5, 5, 255);
		ImGui::GetStyle().Colors[ImGuiCol_PlotLines]						= ImColor(90, 90, 90, 255);
		ImGui::GetStyle().Colors[ImGuiCol_PlotLinesHovered]					= ImColor(255, 255, 255, 255);
		ImGui::GetStyle().Colors[ImGuiCol_PlotHistogram]					= ImColor(90, 90, 90, 255);
		ImGui::GetStyle().Colors[ImGuiCol_PlotHistogramHovered]				= ImColor(255, 255, 255, 255);
		ImGui::GetStyle().Colors[ImGuiCol_TableHeaderBg]					= ImColor(5, 5, 5, 255);
		ImGui::GetStyle().Colors[ImGuiCol_TableBorderStrong]				= ImColor(60, 60, 60, 255);
		ImGui::GetStyle().Colors[ImGuiCol_TableBorderLight]					= ImColor(60, 60, 60, 255);
		ImGui::GetStyle().Colors[ImGuiCol_TableRowBg]						= ImColor(10, 10, 10, 255);
		ImGui::GetStyle().Colors[ImGuiCol_TableRowBgAlt]					= ImColor(50, 50, 50, 255);
		ImGui::GetStyle().Colors[ImGuiCol_TextLink]							= ImColor(66, 150, 250, 255);
		ImGui::GetStyle().Colors[ImGuiCol_TextSelectedBg]					= ImColor(70, 70, 70, 255);
		ImGui::GetStyle().Colors[ImGuiCol_TreeLines]						= ImColor(65, 65, 65, 255);
		ImGui::GetStyle().Colors[ImGuiCol_DragDropTarget]					= ImColor(255, 255, 255, 255);
		ImGui::GetStyle().Colors[ImGuiCol_NavCursor]						= ImColor(255, 255, 255, 255);
		ImGui::GetStyle().Colors[ImGuiCol_NavWindowingHighlight]			= ImColor(255, 255, 255, 179);
		ImGui::GetStyle().Colors[ImGuiCol_NavWindowingDimBg]				= ImColor(204, 204, 204, 51);
		ImGui::GetStyle().Colors[ImGuiCol_ModalWindowDimBg]					= ImColor(12, 12, 12, 40);

		ImGui::GetStyle().FrameRounding = 2.5f;
		ImGui::GetStyle().PopupRounding = 2.5f;
		ImGui::GetStyle().ScrollbarRounding = 2.5f;
		ImGui::GetStyle().GrabRounding = 2.5f;

		ImGui::GetStyle().WindowPadding = ImVec2(10.0f, 10.0f);
		ImGui::GetStyle().FramePadding = ImVec2(10.0f, 5.0f);
		ImGui::GetStyle().ItemSpacing = ImVec2(4.0f, 5.0f);
		ImGui::GetStyle().ItemInnerSpacing = ImVec2(10.0f, 5.0f);
		ImGui::GetStyle().TouchExtraPadding = ImVec2(0.0f, 0.0f);
		ImGui::GetStyle().IndentSpacing = 25.0f;
		ImGui::GetStyle().ScrollbarSize = 15.0f;
		ImGui::GetStyle().GrabMinSize = 19.0f;

		ImGui::GetStyle().WindowBorderSize = 0.0f;
		ImGui::GetStyle().ChildBorderSize = 0.0f;
		ImGui::GetStyle().PopupBorderSize = 2.0f;
		ImGui::GetStyle().FrameBorderSize = 0.0f;

		ImGui::GetStyle().TreeLinesRounding = 1.5f;
		ImGui::GetStyle().TreeLinesSize = 1.5f;

		Application& app = Application::Get();
		GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::GetStyle().WindowRounding = 4.0f;
			ImGui::GetStyle().Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 410");
	}

	void ImGuiLayer::OnDetach()
	{
		DME_PROFILE_FUNCTION()
		
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
		DME_PROFILE_FUNCTION()
		
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiLayer::End()
	{
		DME_PROFILE_FUNCTION()
		
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
