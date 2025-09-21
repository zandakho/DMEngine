#include "dmepch.h"

#include "ConsolePanel.h"
#include "FontLibrary.h"
#include "DME/ImGui/ImGuiDMEEditor.hpp"

#include <ImGui/imgui.h>

namespace DME
{

	void ConsolePanel::OnAttach()
	{
		m_SettingsIcon = Texture2D::Create("Resources/Icons/Console/Console_SettingsIcon_Img.png");
		m_MagnifyingIcon = Texture2D::Create("Resources/Icons/Console/Console_MagnifyingGlassIcon_Img.png");
		m_FilterIcon = Texture2D::Create("Resources/Icons/Console/Console_FilterIcon_Img.png");
		m_BrushIcon = Texture2D::Create("Resources/Icons/Console/Console_BrushIcon_Img.png");
	}

	void ConsolePanel::OnDetach()
	{
		m_SettingsIcon.reset();
		m_MagnifyingIcon.reset();
		m_FilterIcon.reset();
	}

	void ConsolePanel::OnImGuiRender()
	{
		const char* levels[] = { "All", "Info", "Warn", "Error", "Critical" };
		static int currentLevel = 0;
		static float ConsoleWindowFontScale = 1.0f;

		static ImGuiTextFilter logFilter;

		ImGui::Begin("Console", &m_ConsoleRender, ImGuiWindowFlags_NoCollapse);
		if (ImGuiDMEEditor::IconButton("##ConsoleFilterButton", reinterpret_cast<ImTextureID*>(static_cast<uintptr_t>(m_FilterIcon->GetRendererID())), { 28, 28 })) ImGui::OpenPopup("ConsoleFilterWindow");

		ImGui::SameLine();

		if (ImGuiDMEEditor::IconButton("##ConsoleSettingsButton", reinterpret_cast<ImTextureID*>(static_cast<uintptr_t>(m_SettingsIcon->GetRendererID())), { 28, 28 })) ImGui::OpenPopup("ConsoleSettingsWindow");

		ImGui::SameLine();

		ImGui::PushStyleVarX(ImGuiStyleVar_ItemSpacing, -28.0f);
		ImGui::SetNextItemWidth(500.0f);
		logFilter.Draw("##Filter");

		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.059f, 0.059f, 0.059f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.059f, 0.059f, 0.059f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.059f, 0.059f, 0.059f, 1.0f));

		ImGuiDMEEditor::IconButton("##MagnifyingGlass", reinterpret_cast<ImTextureID*>(static_cast<uint64_t>(m_MagnifyingIcon->GetRendererID())), { 28, 28 });

		ImGui::PopStyleColor(3);
		ImGui::PopStyleVar();

		ImGui::SameLine();

		if (ImGuiDMEEditor::IconButton("##ClearButton", reinterpret_cast<ImTextureID*>(static_cast<uint64_t>(m_BrushIcon->GetRendererID())), { 28, 28 })) DME::Log::m_ImGuiSink->Clear();

		if (ImGui::BeginPopup("ConsoleFilterWindow", ImGuiWindowFlags_NoMove))
		{
			if (ImGui::BeginCombo("Log Level", levels[currentLevel]))
			{
				for (int n = 0; n < IM_ARRAYSIZE(levels); n++)
				{
					bool isSelected = (currentLevel == n);
					if (ImGui::Selectable(levels[n], isSelected))
					{
						currentLevel = n;
						switch (n)
						{
						case 0: DME::LogSettings::m_LogFilter = spdlog::level::trace;    break;
						case 1: DME::LogSettings::m_LogFilter = spdlog::level::info;     break;
						case 2: DME::LogSettings::m_LogFilter = spdlog::level::warn;     break;
						case 3: DME::LogSettings::m_LogFilter = spdlog::level::err;      break;
						case 4: DME::LogSettings::m_LogFilter = spdlog::level::critical; break;
						}
					}
					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			ImGui::EndPopup();
		}

		if (ImGui::BeginPopup("ConsoleSettingsWindow", ImGuiWindowFlags_NoMove))
		{
			ImGui::SliderFloat("ConsoleWindowFontScale", &ConsoleWindowFontScale, 0.5f, 1.5f, "%.1f");

			ImGui::EndPopup();
		}

		ImGui::BeginChild("Logs", { 0, 0 }, ImGuiChildFlags_AlwaysUseWindowPadding, ImGuiWindowFlags_AlwaysUseWindowPadding);

		ImGui::SetWindowFontScale(ConsoleWindowFontScale);
		if (DME::Log::m_ImGuiSink)
		{
			const auto& buffer = DME::Log::m_ImGuiSink->GetBuffer();
			for (auto& entry : buffer)
			{
				if (currentLevel != 0 && entry.level < DME::LogSettings::m_LogFilter)
					continue;

				if (!logFilter.PassFilter(entry.message.c_str()))
					continue;

				ImVec4 color;
				switch (entry.level)
				{
				case spdlog::level::trace:    color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); break;
				case spdlog::level::info:     color = ImVec4(0.0f, 1.0f, 0.0f, 1.0f); break;
				case spdlog::level::warn:     color = ImVec4(1.0f, 1.0f, 0.3f, 1.0f); break;
				case spdlog::level::err:      color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f); break;
				case spdlog::level::critical: color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); break;
				}

				ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[FontLibrary::OpenSansBold_21]);
				ImGui::PushStyleColor(ImGuiCol_Text, color);
				ImGui::TextUnformatted(entry.message.c_str());
				ImGui::PopStyleColor();
				ImGui::PopFont();
			}

			if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
				ImGui::SetScrollHereY(1.0f);
		}

		ImGui::EndChild();

		ImGui::End();
	}

	bool ConsolePanel::OnKeyPressed(const KeyPressedEvent& event)
	{
		if (event.IsRepeat())
			return false;

		return false;
	}

	bool ConsolePanel::OnMouseButtonPressed(const MouseButtonEvent& event)
	{
		return false;
	}
	
}
