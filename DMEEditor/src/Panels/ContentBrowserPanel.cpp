#include "dmepch.h"
#include "ContentBrowserPanel.h"

#include "DME/Core/Input.h"

#include "FontLibrary.h"

#include <ImGui/imgui.h>



namespace DME
{
	
	extern const std::filesystem::path g_AssetPath = "assets";

	static float padding = 16.0f;
	static float thumbnailSize = 128.0f;

	ContentBrowserPanel::ContentBrowserPanel() : m_CurrentDirectory(g_AssetPath)
	{
		m_FolderIcon = Texture2D::Create("Resources/Icons/ContentBrowser/FolderIcon.png");
		m_FileIcon = Texture2D::Create("Resources/Icons/ContentBrowser/FileIcon.png");
	}

	void ContentBrowserPanel::OnImGuiRender()
	{
		ImGui::Begin("Content Browser");

		ImGui::BeginChild("##ContentChildBrowser", ImVec2(ImGui::GetContentRegionAvail().x - 30, 29));

		if (m_CurrentDirectory != std::filesystem::path(g_AssetPath))
		{
			if (ImGui::Button("<-"))
			{
				m_CurrentDirectory = m_CurrentDirectory.parent_path();
			}
		}

		ImGui::EndChild();

		ImGui::SameLine();

		if (ImGui::Button("N##Settings"))
			ImGui::OpenPopup("##SettingsWindow");
		if (ImGui::BeginPopup("##SettingsWindow"))
		{
			ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 16, 512);
			ImGui::SliderFloat("Padding", &padding, 0, 32);

			ImGui::EndPopup();
		}

		ImGui::BeginChild("##Directories", { 100, ImGui::GetContentRegionAvail().y }, ImGuiChildFlags_ResizeX);

		ImGui::EndChild();

		ImGui::SameLine();

		ImGui::BeginChild("##ContentObjects", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y));

		float cellSize = thumbnailSize + padding;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / cellSize);
		if (columnCount < 1)
			columnCount = 1;

		ImGui::Columns(columnCount, 0, false);

		for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
		{
			const auto& path = directoryEntry.path();
			auto relativePath = std::filesystem::relative(path, g_AssetPath);
			std::string filenameString = relativePath.filename().string();

			ImGui::PushID(filenameString.c_str());
			Ref<Texture2D> icon = directoryEntry.is_directory() ? m_FolderIcon : m_FileIcon;
			ImTextureID textureID = static_cast<uintptr_t>(icon->GetRendererID());
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			ImGui::ImageButton("Image", textureID, {thumbnailSize, thumbnailSize}, {0, 1}, {1, 0});

			if (ImGui::BeginDragDropSource())
			{
				const wchar_t* itemPath = relativePath.c_str();
				ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
				ImGui::EndDragDropSource();
			}

			ImGui::PopStyleColor();

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (directoryEntry.is_directory())
					m_CurrentDirectory /= path.filename();

			}
			ImGui::TextWrapped(filenameString.c_str());

			ImGui::NextColumn();

			ImGui::PopID();
		}

		ImGui::EndChild();

		ImGui::End();
	}
}