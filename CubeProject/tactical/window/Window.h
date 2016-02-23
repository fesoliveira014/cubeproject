#ifndef _WINDOW_H_
#define _WINDOW_H_

#include "../Common.h"
#include "../utils/Utils.h"

#include "EventHandler.h"

namespace tactical 
{
	namespace window 
	{
		class Window 
		{
		public:
			Window(int width, int height, std::string title);
			~Window();

			void Clear();
			void Update();

			EventHandler* GetEventHandler() { return &m_eventHandler; }

			bool IsOpen() { return m_open; }

		protected:
			bool Initialize();
			void UpdateKeys();
			void UpdateMouse();

			sf::Window* m_window;
			sf::ContextSettings m_settings;

			std::string m_title;
			bool m_open;

			EventHandler m_eventHandler;
		};
	}
}


#endif