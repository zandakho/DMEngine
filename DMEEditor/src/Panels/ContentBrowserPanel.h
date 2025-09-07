#pragma once

#include <filesystem>

#include "DME/Renderer/Texture.h"
#include "DME/ImGui/ImGuiDMEEditor.h"

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

	};

}

