#pragma once

// --- DME/Core/ ---------------------------------------------//
#include "DME/Core/Layer.h"									//
// -------------------------------------------------------------//

// --- DME/Events/ -------------------------------------------//
#include "DME/Events/ApplicationEvent.h"						//
#include "DME/Events/MouseEvent.h"							//
#include "DME/Events/KeyEvent.h"								//
// -------------------------------------------------------------//

namespace DME
{
	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer() = default;

	 virtual void OnAttach() override;
	 virtual void OnDetach() override;
	 virtual void OnEvent(Event& event) override;

	 void Begin();
	 void End();

	 void BlockEvents(bool block) { m_BlockEvents = block; }

	private:
		bool m_BlockEvents = true;
		float m_Time = 0.0f;
	};
}