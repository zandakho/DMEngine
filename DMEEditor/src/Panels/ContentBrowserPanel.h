#pragma once

#include <dme.h>
#include <filesystem>

#include "DME/Renderer/Texture.h"
#include "DME/Events/Event.h"
#include "DME/Events/KeyEvent.h"
#include "DME/Events/MouseEvent.h"

namespace DME
{
	class ContentBrowserPanel : Layer
	{
	public:

		ContentBrowserPanel();

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		void OnEvent(Event& event) override;
		void OnUpdate(TimeStep ts) override;
		void OnImGuiRender() override;

	private:
		bool OnKeyPressed(KeyPressedEvent& event);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& event);

	private:
		
		std::filesystem::path m_CurrentDirectory;
	
		Ref<Texture2D> m_FolderIcon;
		Ref<Texture2D> m_FileIcon;
		Ref<Texture2D> m_BackButtonIcon;
		Ref<Texture2D> m_SettingsButtonIcon;
	};

}

