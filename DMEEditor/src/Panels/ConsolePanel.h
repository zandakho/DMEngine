#pragma once

#include "DME/Core/Layer.h"

#include "DME/Events/KeyEvent.h"
#include "DME/Events/MouseEvent.h"
#include "DME/Renderer/Texture.h"

namespace DME
{
	class ConsolePanel : public Layer
	{
	public: // Constructors and Destructors

		ConsolePanel() = default;

	public: // Helpers (Get&Set)

		bool GetTextureFullPack() const;
		void ClearTexturePack();

	public: // Layer overrides

		void OnAttach() override;
		void OnDetach() override;
		void OnImGuiRender() override;

	public: // Events

		static bool OnKeyPressed(const KeyPressedEvent& event);
		static bool OnMouseButtonPressed(const MouseButtonEvent& event);

	public:

		bool m_ConsoleRender;

	private: // Textures

		Ref<Texture2D> m_SettingsIcon;
		Ref<Texture2D> m_MagnifyingIcon;
		Ref<Texture2D> m_FilterIcon;
		Ref<Texture2D> m_BrushIcon;

	};
}

