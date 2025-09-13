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
		m_SceneIcon = Texture2D::Create("Resources/Icons/ContentBrowser/SceneIcon_Img.png");
		m_ShadersFVIcon = Texture2D::Create("Resources/Icons/ContentBrowser/ShadersFVIcon_Img.png");
		m_OpenFolderIcon = Texture2D::Create("Resources/Icons/ContentBrowser/OpenFolderIcon_Img.png");
		m_CloseFolderIcon = Texture2D::Create("Resources/Icons/ContentBrowser/CloseFolderIcon_Img.png");
	}

	void ContentBrowserPanel::OnAttach()
	{
	}

	void ContentBrowserPanel::OnDetach()
	{

	}

	void ContentBrowserPanel::OnImGuiRender()
	{
		ImGui::Begin("Content Browser");

		if (m_CurrentDirectory != std::filesystem::path(g_AssetPath))
		{
			if (ImGuiDMEEditor::IconButton("##Back button",
				reinterpret_cast<ImTextureID*>(static_cast<uintptr_t>(m_BackButtonIcon->GetRendererID())),
				{ 30, 30 }, { 1.0f,1.0f,1.0f,1.0f }))
			{
				m_CurrentDirectory = m_CurrentDirectory.parent_path();
			}
		}

		ImGui::SameLine();

		ImGui::BeginChild("##ContentChildBrowser", ImVec2(ImGui::GetContentRegionAvail().x - 30, 31));

		ImGui::EndChild();

		ImGui::SameLine();

		if (ImGuiDMEEditor::IconButton("##Settings",
			reinterpret_cast<ImTextureID*>(static_cast<uintptr_t>(m_SettingsButtonIcon->GetRendererID())),
			{ 30, 30 }))
			ImGui::OpenPopup("##SettingsWindow");

		if (ImGui::BeginPopup("##SettingsWindow"))
		{
			ImGui::DragFloat("Thumbnail Size", &thumbnailSize, 0.1f, 80.0f, 120.0f, "%.1f", ImGuiSliderFlags_WrapAround);
			ImGui::DragFloat("Padding", &padding, 0.1f, 14.0f, 20.0f, "%.1f", ImGuiSliderFlags_WrapAround);
			ImGui::EndPopup();
		}

		ImGui::BeginChild("##Directories", { 100, ImGui::GetContentRegionAvail().y }, ImGuiChildFlags_ResizeX | ImGuiChildFlags_AlwaysUseWindowPadding);

		DrawDirectoryTree(g_AssetPath);

		ImGui::EndChild();

		ImGui::SameLine();

		ImGui::BeginChild("##ContentObjects", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y));

		float cellSize = thumbnailSize + padding;
		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / cellSize);
		if (columnCount < 1) columnCount = 1;

		ImGui::Columns(columnCount, 0, false);

		for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
		{
			const auto& path = directoryEntry.path();
			std::string filenameString = path.stem().string();
			std::string filenameWithExt = path.filename().string();
			std::string fileExtension = path.extension().string();

			ImGui::PushID(filenameString.c_str());

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
						ImGuiButtonFlags_None, {1.0f, 1.0f, 0.0f, 1.0f});
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

		bool clicked = ImGui::Selectable(folderName.c_str(), m_CurrentDirectory == directory, ImGuiSelectableFlags_AllowDoubleClick);

		if (clicked)
			m_CurrentDirectory = directory;

		if (ImGui::IsItemClicked())
			openState = !openState;

		if (openState)
		{
			ImGui::Indent(30.0f);
			for (auto& entry : std::filesystem::directory_iterator(directory))
			{
				if (entry.is_directory())
					DrawDirectoryTree(entry.path());
			}
			ImGui::Unindent(30.0f);
		}
	}

	void ContentBrowserPanel::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<KeyPressedEvent>(DME_BIND_EVENT_FN(ContentBrowserPanel::OnKeyPressed));
		dispatcher.Dispatch<MouseButtonPressedEvent>(DME_BIND_EVENT_FN(ContentBrowserPanel::OnMouseButtonPressed));
	}

	void ContentBrowserPanel::OnUpdate(TimeStep ts)
	{

	}

	bool ContentBrowserPanel::OnKeyPressed(KeyPressedEvent& event)
	{
		if (event.IsRepeat())
			return false;

		bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
		bool super = Input::IsKeyPressed(Key::LeftSuper) || Input::IsKeyPressed(Key::RightSuper);
		bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);
		bool alt = Input::IsKeyPressed(Key::LeftAlt) || Input::IsKeyPressed(Key::RightAlt);

		return false;
	}

	bool ContentBrowserPanel::OnMouseButtonPressed(MouseButtonPressedEvent& event)
	{
		switch (event.GetMouseButton())
		{
			case Mouse::ButtonRight:
			{
				DME_CORE_INFO("Pressed: {0}", event.GetName());
			}
		}

		return false;
	}
}