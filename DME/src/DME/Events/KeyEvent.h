#pragma once

#include "DME/Events/Event.h"
#include "DME/Core/KeyCodes.h"

namespace DME
{
	class KeyEvent : public Event
	{
	public:

		inline int GetKeyCode() const { return m_KeyCode; }

		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)

	protected:
		KeyEvent(const KeyCode keycode) : m_KeyCode(keycode) {}

		KeyCode m_KeyCode;
	};

	class KeyPressedEvent : public KeyEvent 
	{
	public:
		KeyPressedEvent(const KeyCode keycode, bool isRepeat = false)
			: KeyEvent(keycode), m_IsRepeat(isRepeat) {
		}

		bool IsRepeat() const { return m_IsRepeat; }

		std::string ToString() const override 
		{
			std::stringstream s_Stream;
			s_Stream << "KeyPressedEvent: " << m_KeyCode << " (repeat = " << m_IsRepeat << ")";
			return s_Stream.str();
		}

		EVENT_CLASS_TYPE(KeyPressed)

	private:
		bool m_IsRepeat;
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