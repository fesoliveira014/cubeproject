#include "Window.h"

namespace tactical
{
    namespace window {
        Window::Window(int width, int height, std::string title)
        {
            m_eventHandler = std::make_shared<EventHandler>();
            m_eventHandler->m_windowSizeState.width = width;
            m_eventHandler->m_windowSizeState.height = height;
            m_eventHandler->m_windowSizeState.aspectRatio = float(width) / float(height);
            m_title = title;

            m_open = Initialize();

            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        }

        Window::~Window()
        {
            m_window->close();
            delete m_window;
        }

        bool Window::Initialize()
        {
            m_settings.depthBits = 24;
            m_settings.stencilBits = 8;
            m_settings.antialiasingLevel = 2;
            m_settings.majorVersion = 4;
            m_settings.minorVersion = 3;

            sf::VideoMode videoMode(m_eventHandler->m_windowSizeState.width, m_eventHandler->m_windowSizeState.height);

            m_window = new sf::Window(videoMode, m_title, sf::Style::Default, m_settings);

            glewExperimental = GL_TRUE;

            if (glewInit() != GLEW_OK || m_window == NULL) {
                return false;
            }

            m_window->setFramerateLimit(0);

            return true;
        }

        void Window::Clear()
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }

        void Window::Update()
        {
            GLenum errorGL = glGetError();
            if (errorGL != GL_NO_ERROR) {
                while (errorGL != GL_NO_ERROR) {
                    LOG << LOGTYPE::LOG_WARNING << "OpenGL error: " + std::to_string(errorGL);
                    errorGL = glGetError();
                }
            }

            m_eventHandler->m_mouseEvent.mouse_button_pressed = false;
            m_eventHandler->m_mouseEvent.mouse_button_released = false;
            m_eventHandler->m_mouseEvent.mouse_wheel_scrolled = false;
            m_eventHandler->m_keyEvent.key_pressed = false;
            m_eventHandler->m_keyEvent.key_released = false;

            sf::Event event;
            while (m_window->pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    m_open = false;
                if (event.type == sf::Event::Resized) {
                    m_eventHandler->m_windowSizeState.width = event.size.width;
                    m_eventHandler->m_windowSizeState.height = event.size.height;
                    m_eventHandler->m_windowSizeState.aspectRatio = float(event.size.width) / float(event.size.height);

                    glViewport(0, 0, m_eventHandler->m_windowSizeState.width, m_eventHandler->m_windowSizeState.height);
                }

                if (event.type == sf::Event::GainedFocus) {
                }

                if (event.type == sf::Event::LostFocus) {
                }

                if (event.type == sf::Event::MouseButtonPressed) {
                    m_eventHandler->m_mouseEvent.mouse_button_pressed = true;
                }

                if (event.type == sf::Event::MouseButtonReleased) {
                    m_eventHandler->m_mouseEvent.mouse_button_released = true;
                }

                if (event.type == sf::Event::MouseWheelScrolled) {
                    if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
                        m_eventHandler->m_mouseEvent.mouse_wheel_scrolled = true;                        
                    }
                    m_eventHandler->m_mouseState.mouse_scroll_delta = event.mouseWheelScroll.delta;
                }

                if (event.type == sf::Event::KeyPressed) {
                    m_eventHandler->m_keyEvent.key_pressed = true;
                }

                if (event.type == sf::Event::KeyReleased) {
                    m_eventHandler->m_keyEvent.key_released = true;                    
                }
            }

            if (m_window->hasFocus()) {
                UpdateKeys();
                UpdateMouse();
            }

            m_window->display();
        }

        void Window::UpdateKeys()
        {
            m_eventHandler->m_keyboardState.key_1 = sf::Keyboard::isKeyPressed(sf::Keyboard::Num1);
            m_eventHandler->m_keyboardState.key_2 = sf::Keyboard::isKeyPressed(sf::Keyboard::Num2);
            m_eventHandler->m_keyboardState.key_3 = sf::Keyboard::isKeyPressed(sf::Keyboard::Num3);

            m_eventHandler->m_keyboardState.key_a = sf::Keyboard::isKeyPressed(sf::Keyboard::A);
            m_eventHandler->m_keyboardState.key_s = sf::Keyboard::isKeyPressed(sf::Keyboard::S);
            m_eventHandler->m_keyboardState.key_d = sf::Keyboard::isKeyPressed(sf::Keyboard::D);
            m_eventHandler->m_keyboardState.key_w = sf::Keyboard::isKeyPressed(sf::Keyboard::W);
            m_eventHandler->m_keyboardState.key_q = sf::Keyboard::isKeyPressed(sf::Keyboard::Q);
            m_eventHandler->m_keyboardState.key_e = sf::Keyboard::isKeyPressed(sf::Keyboard::E);
            m_eventHandler->m_keyboardState.key_f = sf::Keyboard::isKeyPressed(sf::Keyboard::F);
            m_eventHandler->m_keyboardState.key_c = sf::Keyboard::isKeyPressed(sf::Keyboard::C);

            m_eventHandler->m_keyboardState.key_enter = sf::Keyboard::isKeyPressed(sf::Keyboard::Return);
            m_eventHandler->m_keyboardState.key_esc = sf::Keyboard::isKeyPressed(sf::Keyboard::Escape);
            m_eventHandler->m_keyboardState.key_shift = sf::Keyboard::isKeyPressed(sf::Keyboard::LShift);
            m_eventHandler->m_keyboardState.key_space = sf::Keyboard::isKeyPressed(sf::Keyboard::Space);
        }

        void Window::UpdateMouse()
        {
            m_eventHandler->m_mouseState.mouse_button_left = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
            m_eventHandler->m_mouseState.mouse_button_right = sf::Mouse::isButtonPressed(sf::Mouse::Button::Right);
            m_eventHandler->m_mouseState.mouse_button_middle = sf::Mouse::isButtonPressed(sf::Mouse::Button::Middle);

            m_eventHandler->m_mouseState.mouse_x = sf::Mouse::getPosition(*m_window).x;
            m_eventHandler->m_mouseState.mouse_y = sf::Mouse::getPosition(*m_window).y;
        }

        void Window::SetTitle(std::string title)
        {
            m_title = title;
            m_window->setTitle(title);
        }
    }
}