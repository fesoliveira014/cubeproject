#include "EventHandler.h"

namespace tactical
{
	namespace window
	{
		EventHandler::EventHandler()
		{
			m_keyboardState.key_1 = false;
			m_keyboardState.key_2 = false;
			m_keyboardState.key_3 = false;
			m_keyboardState.key_4 = false;
			m_keyboardState.key_5 = false;
			m_keyboardState.key_6 = false;
			m_keyboardState.key_7 = false;
			m_keyboardState.key_8 = false;
			m_keyboardState.key_9 = false;

			m_keyboardState.key_a = false;
			m_keyboardState.key_s = false;
			m_keyboardState.key_d = false;
			m_keyboardState.key_w = false;
			m_keyboardState.key_q = false;
			m_keyboardState.key_e = false;
			m_keyboardState.key_f = false;
			m_keyboardState.key_c = false;

			m_keyboardState.key_enter = false;
			m_keyboardState.key_esc   = false;
			m_keyboardState.key_shift = false;
			m_keyboardState.key_space = false;
		}

		EventHandler::~EventHandler()
		{

		}

		void EventHandler::Update()
		{
			
		}

		KeyboardState* EventHandler::GetKeyboardState()
		{
			return &m_keyboardState;
		}

		MouseState* EventHandler::GetMouseState()
		{
			return &m_mouseState;
		}

		WindowSizeState* EventHandler::GetWindowSizeState()
		{
			return &m_windowSizeState;
		}

		MouseEvent* EventHandler::GetMouseEvent()
		{
			return &m_mouseEvent;
		}

		KeyEvent* EventHandler::GetKeyEvent()
		{
			return &m_keyEvent;
		}
	}
}