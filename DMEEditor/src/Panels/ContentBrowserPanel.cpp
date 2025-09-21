#include "dmepch.h"

#include "ContentBrowserPanel.h"

#include "DME/Core/Input.h"

#include "DME/ImGui/ImGuiDMEEditor.hpp"

#include <ImGui/imgui.h>
#include <ImGui/imgui_internal.h>

namespace DME
{
	
	extern const std::filesystem::path g_AssetPath = "assets";

	static float padding = 20.0f;
	static float thumbnailSize = 100.0f;

	static void IsCheckerTextError(bool check, const ImVec4& color, const char* format, ...)
	{
		if (check)
			return;

		va_list args;
		va_start(args, format);
		ImGui::TextColoredV(color, format, args);
		va_end(args);
	}

	ContentBrowserPanel::ContentBrowserPanel() : m_CurrentDirectory(g_AssetPath)
	{
	}

	void ContentBrowserPanel::OnAttach()
	{
		m_FolderIcon = Texture2D::Create("Resources/Icons/ContentBrowser/CB_FolderIcon_Img.png");
		m_FileIcon = Texture2D::Create("Resources/Icons/ContentBrowser/CB_FileIcon_Img.png");
		m_BackIcon = Texture2D::Create("Resources/Icons/ContentBrowser/CB_BackIcon_Img.png");
		m_SettingsIcon = Texture2D::Create("Resources/Icons/ContentBrowser/CB_SettingsIcon_Img.png");
		m_SceneIcon = Texture2D::Create("Resources/Icons/ContentBrowser/CB_SceneIcon_Img.png");
		m_ShadersFVIcon = Texture2D::Create("Resources/Icons/ContentBrowser/CB_ShadersFVIcon_Img.png");
		m_OpenFolderIcon = Texture2D::Create("Resources/Icons/ContentBrowser/CB_OpenFolderIcon_Img.png");
		m_CloseFolderIcon = Texture2D::Create("Resources/Icons/ContentBrowser/CB_CloseFolderIcon_Img.png");
		m_PlusSmallIcon = Texture2D::Create("Resources/Icons/ContentBrowser/CB_PlusSmallIcon_Img.png");
	}

	void ContentBrowserPanel::OnDetach()
	{
		m_FolderIcon.reset();
		m_FileIcon.reset();
		m_BackIcon.reset();
		m_SettingsIcon.reset();
		m_SceneIcon.reset();
		m_ShadersFVIcon.reset();
		m_CloseFolderIcon.reset();
		m_PlusSmallIcon.reset();
		m_OpenFolderIcon.reset();
		m_TextureCache.clear();
	}

	void ContentBrowserPanel::OnImGuiRender()
	{
		ImGui::Begin("Content Browser", &m_ContentBrowserRender, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysUseWindowPadding);

		ImGui::BeginChild("##Directories", { 100, 0 }, ImGuiChildFlags_ResizeX | ImGuiChildFlags_AlwaysUseWindowPadding);

		DrawDirectoryTree(g_AssetPath);

		ImGui::EndChild();

		ImGui::SameLine();

		ImGui::BeginChild("##ContentObjects", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), ImGuiChildFlags_AlwaysUseWindowPadding);

		if (m_CurrentDirectory != std::filesystem::path(g_AssetPath))
		{
			if (ImGuiDMEEditor::IconButton("##Back button",
				reinterpret_cast<ImTextureID*>(static_cast<uintptr_t>(m_BackIcon->GetRendererID())),
				{ 28, 28 }, { 1.0f,1.0f,1.0f,1.0f }))
			{
				m_CurrentDirectory = m_CurrentDirectory.parent_path();
			}
		}

		ImGui::SameLine();

		if (ImGuiDMEEditor::IconButtonWithText("ADD##ContentChildBrowser", reinterpret_cast<ImTextureID*>(static_cast<uint64_t>(m_PlusSmallIcon->GetRendererID()))))
		{
			strcpy_s(m_NewFolderName, "New Folder");
			ImGui::OpenPopup("AddItemDialogWindow");
		}

		if (ImGui::BeginPopup("AddItemDialogWindow"))
		{
			ImGui::SeparatorText("New item");
			ImGui::Separator();

			if (ImGui::Button("Add new folder"))
			{
				ImGui::OpenPopup("ModalWindow");
			}

			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 4.0f);
			if (ImGui::BeginPopupModal("ModalWindow", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar))
			{
				std::filesystem::path newFolder = m_CurrentDirectory / m_NewFolderName;
				ImGui::InputText("Folder name", m_NewFolderName, IM_ARRAYSIZE(m_NewFolderName));

				bool isNameEmpty = strlen(m_NewFolderName) == 0;
				bool folderAlreadyExists = std::filesystem::exists(newFolder);
				bool isSameAsCurrent = (newFolder == m_CurrentDirectory);

				bool canCreateFolder = !isNameEmpty && !folderAlreadyExists && !isSameAsCurrent;

				IsCheckerTextError(!isNameEmpty, ImVec4(1, 0.0f, 0, 1), "Folder name cannot be empty");

				IsCheckerTextError(!folderAlreadyExists, ImVec4(1, 0.0f, 0, 1), "A folder with this name already exists");

				IsCheckerTextError(!isSameAsCurrent, ImVec4(1, 0.0f, 0, 1), "Cannot create folder with current directory name");

				ImGui::BeginDisabled(!canCreateFolder);
				if (ImGui::Button("Add"))
				{

					if (!std::filesystem::exists(newFolder))
						std::filesystem::create_directory(newFolder);

					ImGui::CloseCurrentPopup();
				}
				ImGui::EndDisabled();

				ImGui::SameLine();

				if (ImGui::Button("Cancel"))
				{
					ImGui::CloseCurrentPopup();

				}

				if (!ImGui::IsWindowHovered() && ImGui::IsMouseDoubleClicked(0))
					ImGui::CloseCurrentPopup();

				ImGui::EndPopup();
			}

			ImGui::PopStyleVar();

			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
				ImGui::CloseCurrentPopup();

			ImGui::EndPopup();
		}


		ImGui::SameLine();

		if (ImGuiDMEEditor::IconButton("##Settings",
			reinterpret_cast<ImTextureID*>(static_cast<uintptr_t>(m_SettingsIcon->GetRendererID())),
			{ 28, 28 }))
			ImGui::OpenPopup("##SettingsWindow");

		if (ImGui::BeginPopup("##SettingsWindow"))
		{
			ImGui::DragFloat("Thumbnail Size", &thumbnailSize, 0.1f, 80.0f, 120.0f, "%.1f", ImGuiSliderFlags_WrapAround);
			ImGui::DragFloat("Padding", &padding, 0.1f, 14.0f, 20.0f, "%.1f", ImGuiSliderFlags_WrapAround);

			ImGui::EndPopup();
		}

		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);

		float cellSize = thumbnailSize + padding;
		float panelWidth = ImGui::GetContentRegionAvail().x;
		auto columnCount = static_cast<int>(panelWidth / cellSize);
		if (columnCount < 1) columnCount = 1;

		ImGui::Columns(columnCount, 0, false);

		for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
		{
			const auto& path = directoryEntry.path();
			std::string filenameString = path.stem().string();
			std::string filenameWithExt = path.filename().string();
			std::string fileExtension = path.extension().string();
			std::string PushID = std::format("{0}-{1}", filenameString, fileExtension);

			ImGui::PushID(PushID.c_str());

			if (directoryEntry.is_directory())
			{
				ImGuiDMEEditor::FolderCard(filenameString.c_str(),
					reinterpret_cast<void*>(static_cast<uintptr_t>(m_FolderIcon->GetRendererID())),
					nullptr, nullptr, { 0,1 }, { 1,0 }, ImGuiButtonFlags_None);
			}
			else
			{
				auto ext = path.extension().string();
				std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

				if (ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".bmp" || ext == ".tga")
				{
					Ref<Texture2D> texture;
					auto it = m_TextureCache.find(path.string());
					if (it != m_TextureCache.end())
						texture = it->second;
					else
					{
						texture = Texture2D::Create(path.string());
						m_TextureCache[path.string()] = texture;
					}

					ImGuiDMEEditor::CardWithAssetType(
						filenameString.c_str(),
						reinterpret_cast<void*>(static_cast<uintptr_t>(texture->GetRendererID())),
						"Texture",
						fileExtension.c_str(),
						path.string().c_str(),
						{ 0,1 }, { 1,0 },
						glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
						ImGuiButtonFlags_None, { 1.0f, 0.0f, 0.0f, 1.0f });
				}
				else if (ext == ".dme")
				{
					ImGuiDMEEditor::CardWithAssetType(
						filenameString.c_str(),
						reinterpret_cast<void*>(static_cast<uintptr_t>(m_SceneIcon->GetRendererID())),
						"Scene",
						fileExtension.c_str(),
						path.string().c_str(),
						{ 0,1 }, { 1,0 },
						glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
						ImGuiButtonFlags_None, {1.0f, 0.5f, 0.0f, 1.0f});
				}
				else if (ext == ".glsl" || ext == ".hlsl" || ext == ".frag" || ext == ".vert")
				{
					ImGuiDMEEditor::CardWithAssetType(
						filenameString.c_str(),
						reinterpret_cast<void*>(static_cast<uintptr_t>(m_ShadersFVIcon->GetRendererID())),
						"Graphics Shader",
						fileExtension.c_str(),
						path.string().c_str(),
						{ 0,1 }, { 1,0 },
						glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
						ImGuiButtonFlags_None, glm::vec4(0.5f, 0.5f, 1.0f, 1.0f));
				}
				else
				{
					ImGuiDMEEditor::CardWithAssetType(
						filenameString.c_str(),
						reinterpret_cast<void*>(static_cast<uintptr_t>(m_FileIcon->GetRendererID())),
						"File",
						fileExtension.c_str(),
						path.string().c_str(),
						{ 0,1 }, { 1,0 },
						glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
						ImGuiButtonFlags_None);
				}
			}

			if (ImGui::BeginDragDropSource())
			{
				auto relativePath = std::filesystem::relative(path, g_AssetPath);
				const wchar_t* itemPath = relativePath.c_str();
				ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath,
					(wcslen(itemPath) + 1) * sizeof(wchar_t));
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

	void ContentBrowserPanel::DrawDirectoryTree(const std::filesystem::path& directory)
	{
		std::string folderName = directory.filename().string();
		if (folderName.empty()) folderName = directory.string();

		bool& openState = m_FolderStates[directory.string()];

		ImGui::Image(
			reinterpret_cast<ImTextureID*>(static_cast<uintptr_t>(
				openState ? m_OpenFolderIcon->GetRendererID() : m_CloseFolderIcon->GetRendererID())),
			ImVec2(16, 16), { 0, 1 }, {1, 0});
		ImGui::SameLine();

		if (ImGui::Selectable(folderName.c_str(), m_CurrentDirectory == directory, ImGuiSelectableFlags_AllowDoubleClick | ImGuiSelectableFlags_SpanAvailWidth))
			m_CurrentDirectory = directory;

		if (ImGui::IsItemClicked())
			openState = !openState;

		if (openState)
		{
			ImGui::Indent(20.0f);
			for (auto& entry : std::filesystem::directory_iterator(directory))
			{
				if (entry.is_directory())
					DrawDirectoryTree(entry.path());
			}
			ImGui::Unindent(20.0f);
		}
	}


	bool ContentBrowserPanel::OnKeyPressed(const KeyPressedEvent& event)
	{
		if (event.IsRepeat())
			return false;

		switch (event.GetKeyCode())
		{
			case Key::A:
				DME_INFO("Pressed {}", event.GetName())
		}

		return false;
	}

	bool ContentBrowserPanel::OnMouseButtonPressed(const MouseButtonPressedEvent& event)
	{
		return false;
	}
}
