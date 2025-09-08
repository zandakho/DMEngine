#include "dmepch.h"
#include "ContentBrowserPanel.h"

#include "DME/Core/Input.h"

#include "FontLibrary.h"

#include "DME/ImGui/ImGuiDMEEditor.h"

#include <ImGui/imgui.h>



namespace DME
{
	
	extern const std::filesystem::path g_AssetPath = "assets";

	static float padding = 16.0f;
	static float thumbnailSize = 101.0f;

	ContentBrowserPanel::ContentBrowserPanel() : m_CurrentDirectory(g_AssetPath)
	{
		m_FolderIcon = Texture2D::Create("Resources/Icons/ContentBrowser/FolderIcon.png");
		m_FileIcon = Texture2D::Create("Resources/Icons/ContentBrowser/FileIcon.png");
		m_BackButtonIcon = Texture2D::Create("Resources/Icons/ContentBrowser/Back_Img.png");
		m_SettingsButtonIcon = Texture2D::Create("Resources/Icons/ContentBrowser/Settings_Img.png");
	}

	void ContentBrowserPanel::OnImGuiRender()
	{
		ImTextureID textureID = static_cast<uintptr_t>(m_BackButtonIcon->GetRendererID());
		ImTextureID SettingsButtonID = static_cast<uintptr_t>(m_SettingsButtonIcon->GetRendererID());

		ImGui::Begin("Content Browser");

		ImGui::BeginChild("##ContentChildBrowser", ImVec2(ImGui::GetContentRegionAvail().x - 30, 31));

		if (m_CurrentDirectory != std::filesystem::path(g_AssetPath))
		{
			if (ImGuiDMEEditor::IconButton("##Back button", reinterpret_cast<ImTextureID*>(textureID), { 30, 30 }, { 1.0f, 1.0f, 1.0f, 1.0f }))
			{
				m_CurrentDirectory = m_CurrentDirectory.parent_path();
			}
		}

		ImGui::EndChild();

		ImGui::SameLine();

		if (ImGuiDMEEditor::IconButton("##Settings", reinterpret_cast<ImTextureID*>(SettingsButtonID), { 30, 30 }))
			ImGui::OpenPopup("##SettingsWindow");
		if (ImGui::BeginPopup("##SettingsWindow"))
		{
			ImGui::DragFloat("Thumbnail Size", &thumbnailSize, 0.1f, 0.0f);
			ImGui::DragFloat("Padding", &padding, 0.1f, 32);

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
			std::string filenameString = path.filename().string();

			ImGui::PushID(filenameString.c_str());
			Ref<Texture2D> folder_icon = m_FolderIcon;
			Ref<Texture2D> file_icon = m_FileIcon;
			ImTextureID FolderTextureID = static_cast<uintptr_t>(folder_icon->GetRendererID());
			ImTextureID FileTextureID = static_cast<uintptr_t>(file_icon->GetRendererID());

			if (directoryEntry.is_directory())
			{
				ImGuiDMEEditor::FolderCard(filenameString.c_str(), reinterpret_cast<void*>(FolderTextureID), nullptr, nullptr, { 0, 1 }, {1, 0}, ImGuiButtonFlags_None);
			}
			else
			{
				ImGuiDMEEditor::CardWithAssetType(filenameString.c_str(), reinterpret_cast<void*>(FileTextureID), "Texture", "Not hint", {0, 1}, {1, 0}, glm::vec4(0.25f, 0.25f, 0.25f, 1.0f), ImGuiButtonFlags_None);
			}

			if (ImGui::BeginDragDropSource())
			{
				auto relativePath = std::filesystem::relative(path, g_AssetPath);
				const wchar_t* itemPath = relativePath.c_str();
				ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
				ImGui::EndDragDropSource();
			}

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (directoryEntry.is_directory())
					m_CurrentDirectory /= path.filename();

			}
			
			ImGui::NextColumn();

			ImGui::PopID();
		}
		

		ImGui::EndChild();

		ImGui::End();
	}
}