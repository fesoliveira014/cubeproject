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

			bool key_1;
			bool key_2;
			bool key_3;

			bool key_esc;
			bool key_enter;
			bool key_space;
			bool key_shift;
		};

		struct MouseState {
			bool mouse_button_left;
			bool mouse_button_right;
			bool mouse_button_middle;
			double mouse_x;
			double mouse_y;
		};

		struct WindowSizeState {
			int width;
			int height;
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

		protected:
			KeyboardState m_keyboardState;
			MouseState m_mouseState;
			WindowSizeState m_windowSizeState;

			//sf::Clock m_timer;
		};

	}
}

#endif