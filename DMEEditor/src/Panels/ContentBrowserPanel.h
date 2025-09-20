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

	public: // Helpers (Get&Set)

		bool GetTextureFullPack() const;
		void ClearTexturePack();

	public: // Layer overrides

		void OnAttach() override;
		void OnDetach() override;
		void OnImGuiRender() override;

	public: // UI

		void DrawDirectoryTree(const std::filesystem::path& directory);

	public: // Events

		static bool OnKeyPressed(const KeyPressedEvent& event);
		static bool OnMouseButtonPressed(const MouseButtonPressedEvent& event);

	public:

		bool m_ContentBrowserRender;

	private: // Directory
		
		std::filesystem::path m_CurrentDirectory;
	
	private: // Textures

		Ref<Texture2D> m_FolderIcon;
		Ref<Texture2D> m_FileIcon;
		Ref<Texture2D> m_BackButtonIcon;
		Ref<Texture2D> m_SettingsButtonIcon;
		Ref<Texture2D> m_SceneIcon;
		Ref<Texture2D> m_ShadersFVIcon;
		Ref<Texture2D> m_CloseFolderIcon;
		Ref<Texture2D> m_PlusSmallGreenIcon;
		Ref<Texture2D> m_OpenFolderIcon;

		std::unordered_map<std::string, Ref<Texture2D>> m_TextureCache;

	private: // Folders

		std::unordered_map<std::string, bool> m_FolderStates;

		char m_NewFolderName[128] = "New Folder";
		bool m_CreateFolderPopupOpen = false;
	};

}

