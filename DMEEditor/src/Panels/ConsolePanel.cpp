#include "dmepch.h"

#include "ConsolePanel.h"
#include "FontLibrary.h"
#include "DME/ImGui/ImGuiDMEEditor.h"

#include <ImGui/imgui.h>

namespace DME
{

	ConsolePanel::ConsolePanel() {}

	Ref<Texture2D> ConsolePanel::GetTextureFullPack() const
	{
		return m_SettingButton;
	}

	void ConsolePanel::ClearTexturePack()
	{
		m_SettingButton = nullptr;
	}
	
	void ConsolePanel::OnAttach()
	{
		m_SettingButton = Texture2D::Create("Resources/Icons/Console/SettingsIcon_Img.png");
	}

	void ConsolePanel::OnDetach()
	{
		ClearTexturePack();
	}

	void ConsolePanel::OnImGuiRender()
	{

		if (!GetTextureFullPack())
			return;

		const char* levels[] = { "All", "Info", "Warn", "Error", "Critical" };
		static int currentLevel = 0;
		static float ConsoleWindowFontScale = 1.0f;

		if (ImGui::Begin("Console"))
		{

			if (ImGuiDMEEditor::IconButton("##ConsoleSettingsButton", reinterpret_cast<ImTextureID*>(static_cast<uintptr_t>(m_SettingButton->GetRendererID())), { 30, 30 }))
				ImGui::OpenPopup("ConsoleSettingsWindow");

			if (ImGui::BeginPopup("ConsoleSettingsWindow", ImGuiWindowFlags_NoMove))
			{

				if (ImGui::Button("Clear")) DME::Log::m_ImGuiSink->Clear();

				ImGui::SliderFloat("ConsoleWindowFontScale", &ConsoleWindowFontScale, 0.5f, 1.5f, "%.1f");

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

			if (ImGui::BeginChild("Logs"))
			{
				ImGui::SetWindowFontScale(ConsoleWindowFontScale);
				if (DME::Log::m_ImGuiSink)
				{
					const auto& buffer = DME::Log::m_ImGuiSink->GetBuffer();
					for (auto& entry : buffer)
					{
						if (currentLevel != 0)
						{
							if (entry.level < DME::LogSettings::m_LogFilter)
								continue;
						}

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
						ImGui::Text(entry.message.c_str());
						ImGui::PopStyleColor();
						ImGui::PopFont();
					}

					if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
						ImGui::SetScrollHereY(1.0f);
				}
			}
			ImGui::EndChild();
		}
		ImGui::End();
	}

	bool ConsolePanel::OnKeyPressed(KeyPressedEvent& event)
	{
		if (event.IsRepeat())
			return false;

		return false;
	}

	bool ConsolePanel::OnMouseButtonPressed(MouseButtonEvent& event)
	{
		return false;
	}
	
}
