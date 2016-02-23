#include "EventHandler.h"

namespace tactical
{
	namespace window
	{
		EventHandler::EventHandler()
		{
			//m_timer.restart();
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
	}
}