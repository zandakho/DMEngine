#pragma once

#include "DME/Events/Event.h"

namespace DME
{
	class KeyEvent : public Event
	{
	public:

		inline int GetKeyCode() const { return m_KeyCode; }

		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)

	protected:
		KeyEvent(int keycode) : m_KeyCode(keycode) {}

		int m_KeyCode;
	};

	class KeyPressedEvent : public KeyEvent 
	{
	public:
		KeyPressedEvent(int keycode, int repeatcount) : KeyEvent(keycode), m_Repeatcount(repeatcount) {}

		inline int GetRepeatCount() const { return m_Repeatcount; }

		std::string ToString() const override 
		{
			std::stringstream s_Stream;
			s_Stream << "KeyPressedEvent: " << m_KeyCode << ", repeat: " << "( " << m_Repeatcount << " )";
			return s_Stream.str();
		}

		EVENT_CLASS_TYPE(KeyPressed)
	private:
		int m_Repeatcount;
	};

	class KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(int keycode) : KeyEvent(keycode) {}

		std::string ToString() const override
		{
			std::stringstream s_Stream;
			s_Stream << "KeyReleasedEvent: " << m_KeyCode;
			return s_Stream.str();
		}

		EVENT_CLASS_TYPE(KeyReleased)
	};

	class KeyTypedEvent : public KeyEvent
	{
	public:
		KeyTypedEvent(int keycode) : KeyEvent(keycode) {}

		std::string ToString() const override
		{
			std::stringstream s_Stream;
			s_Stream << "KeyTypedEvent: " << m_KeyCode;
			return s_Stream.str();
		}

		EVENT_CLASS_TYPE(KeyTyped)
	};
	
}