#pragma once

#include <filesystem>


namespace DME
{
	class ContentBrowserPanel
	{
	public:

		ContentBrowserPanel();

		void OnImGuiRender();

	private:
		
		std::filesystem::path m_CurrentDirectory;
	};

}

