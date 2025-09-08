#pragma once

#include <filesystem>

#include "DME/Renderer/Texture.h"

namespace DME
{
	class ContentBrowserPanel
	{
	public:

		ContentBrowserPanel();

		void OnImGuiRender();

	private:
		
		std::filesystem::path m_CurrentDirectory;
	
		Ref<Texture2D> m_FolderIcon;
		Ref<Texture2D> m_FileIcon;
		Ref<Texture2D> m_BackButtonIcon;
		Ref<Texture2D> m_SettingsButtonIcon;
	};

}

