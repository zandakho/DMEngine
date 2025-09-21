#pragma once

#include <dme.h>
#include <filesystem>

#include "DME/Events/KeyEvent.h"
#include "DME/Events/MouseEvent.h"
#include "DME/Renderer/Texture.h"

namespace DME
{
	struct FolderNode 
	{
		std::filesystem::path path;
		bool open = false;
	};

	class ContentBrowserPanel : Layer
	{
	public: // Constructors and Destructors
		
		ContentBrowserPanel();

	public: // Layer overrides

		void OnAttach() override;
		void OnDetach() override;
		void OnImGuiRender() override;

	public: // UI

		void DrawDirectoryTree(const std::filesystem::path& directory);

	public: // Events

		bool OnKeyPressed(const KeyPressedEvent& event);
		bool OnMouseButtonPressed(const MouseButtonPressedEvent& event);

	public:

		bool m_ContentBrowserRender = true;

	private: // Directory
		
		std::filesystem::path m_CurrentDirectory;
	
	private: // Textures

		Ref<Texture2D> m_FolderIcon;
		Ref<Texture2D> m_FileIcon;
		Ref<Texture2D> m_BackIcon;
		Ref<Texture2D> m_SettingsIcon;
		Ref<Texture2D> m_SceneIcon;
		Ref<Texture2D> m_ShadersFVIcon;
		Ref<Texture2D> m_OpenFolderIcon;
		Ref<Texture2D> m_CloseFolderIcon;
		Ref<Texture2D> m_PlusSmallIcon;

		std::unordered_map<std::string, Ref<Texture2D>> m_TextureCache;

	private: // Folders

		std::unordered_map<std::string, bool> m_FolderStates;

		char m_NewFolderName[128] = "New Folder";
		bool m_CreateFolderPopupOpen = false;
	};

}

