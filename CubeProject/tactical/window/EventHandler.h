#ifndef _EVENT_HANDLER_H_
#define _EVENT_HANDLER_H_

#include "../Common.h"
#include "../utils/Utils.h"

namespace tactical
{
	namespace window
	{
		class Window;

		struct KeyboardState {
			bool key_a;
			bool key_s;
			bool key_d;
			bool key_w;
			bool key_c;
			bool key_q;
			bool key_e;
			bool key_f;

			bool key_1;
			bool key_2;
			bool key_3;
			bool key_4;
			bool key_5;
			bool key_6;
			bool key_7;
			bool key_8;
			bool key_9;

			bool key_esc;
			bool key_enter;
			bool key_space;
			bool key_shift;
		};

		struct KeyEvent {
			bool key_pressed;
			bool key_released;
		};

		struct MouseState {
			bool mouse_button_left;
			bool mouse_button_right;
			bool mouse_button_middle;
			double mouse_x;
			double mouse_y;
            double mouse_scroll_delta;
		};

		struct MouseEvent {
			bool mouse_button_pressed;
			bool mouse_button_released;
            bool mouse_wheel_scrolled;
		};

		struct WindowSizeState {
			int width;
			int height;
			float aspectRatio;
		};

		class EventHandler
		{
		public:
			friend class Window;

			EventHandler();
			~EventHandler();

			void Update();

			KeyboardState*   GetKeyboardState();
			MouseState*		 GetMouseState();
			WindowSizeState* GetWindowSizeState();
			MouseEvent*		 GetMouseEvent();
			KeyEvent*		 GetKeyEvent();

		protected:
			KeyboardState m_keyboardState;
			KeyEvent m_keyEvent;
			WindowSizeState m_windowSizeState;
			MouseState m_mouseState;
			MouseEvent m_mouseEvent;

			//sf::Clock m_timer;
		};

	}
}

#endif