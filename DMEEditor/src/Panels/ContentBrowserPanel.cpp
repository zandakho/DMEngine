#include "dmepch.h"
#include "ContentBrowserPanel.h"

#include "FontLibrary.h"

#include <ImGui/imgui.h>



namespace DME
{

	static const std::filesystem::path s_AssetPath = "assets";

	ContentBrowserPanel::ContentBrowserPanel() : m_CurrentDirectory(s_AssetPath)
	{
		
	}

	void ContentBrowserPanel::OnImGuiRender()
	{
		ImGui::Begin("Content Browser");

		ImGui::BeginChild("##ContentChildBrowser", ImVec2(ImGui::GetContentRegionAvail().x, 29));

		if (m_CurrentDirectory != std::filesystem::path(s_AssetPath))
		{
			if (ImGui::Button("<-"))
			{
				m_CurrentDirectory = m_CurrentDirectory.parent_path();
			}
		}


		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[static_cast<int>(FontLibrary::NotoSansDevanagariRegular_21)]);
		for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
		{
			const auto& path = directoryEntry.path();
			auto relativePath = std::filesystem::relative(path, s_AssetPath);
			std::string filenameString = relativePath.filename().string();
			if (directoryEntry.is_directory())
			{
				ImGui::SameLine();
				if (ImGui::Button(filenameString.c_str()))
				{
					m_CurrentDirectory /= path.filename();
				}

			}
		}
		ImGui::PopFont();

		ImGui::EndChild();

		ImGui::BeginChild("##ContentAssets", ImVec2(150, ImGui::GetContentRegionAvail().y), ImGuiChildFlags_ResizeX);

		ImGui::EndChild();

		ImGui::SameLine();

		ImGui::BeginChild("##ContentObjects", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y));

		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[static_cast<int>(FontLibrary::NotoSansDevanagariRegular_21)]);
		for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
		{
			const auto& path = directoryEntry.path();
			auto relativePath = std::filesystem::relative(path, s_AssetPath);
			std::string filenameString = relativePath.filename().string();
			if (!directoryEntry.is_directory())
			{
				if (ImGui::Button(filenameString.c_str()))
				{

				}
			}
		}
		ImGui::PopFont();

		ImGui::EndChild();

		

		ImGui::End();
	}
}