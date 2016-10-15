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

            std::shared_ptr<EventHandler> GetEventHandler() { return m_eventHandler; }

            bool IsOpen() { return m_open; }

            void SetTitle(std::string title);
            std::string GetTitle() { return m_title; }

        protected:
            bool Initialize();
            void UpdateKeys();
            void UpdateMouse();

            sf::Window* m_window;
            sf::ContextSettings m_settings;

            std::string m_title;
            bool m_open;

            std::shared_ptr<EventHandler> m_eventHandler;
        };
    }
}

#endif