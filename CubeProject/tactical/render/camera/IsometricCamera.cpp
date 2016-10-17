#include "IsometricCamera.h"

namespace tactical
{
    namespace render
    {
        IsometricCamera::IsometricCamera(const glm::mat4& projection, const glm::vec3& position, const glm::vec3& target)
            : Camera(projection, position, target)
        {
            Initialize();
        }

        IsometricCamera::~IsometricCamera()
        {
        }

        void IsometricCamera::Initialize()
        {
            m_pitch = glm::radians(45.0f);
            // needs to yaw -90 degrees because of the quadradnt the scene is being drawed,
            // plus -arctan(1/sqr(2)) =~ -35.264 degrees
            m_yaw = glm::radians(-125.264f);
            m_speed = 6.0f;
            m_rotationSpeed = 0.003f;
            m_zoom = 1.0f;
        }

        void IsometricCamera::Update(float deltaTime)
        {
            UpdateStates();
            UpdateZoom();
            UpdateProjection();

            float velocity = m_speed * deltaTime;
            Move(velocity);

            MouseRotate(m_delta);

            m_position += m_translation;
            m_translation = glm::vec3(0.0f);

            // update orthonormal basis
            UpdateBasis();
        }

        void IsometricCamera::Move(float speed)
        {
            if (m_moveState.up) Lift(speed);
            if (m_moveState.down) Lift(-speed);
            if (m_moveState.left) Strafe(-speed);
            if (m_moveState.right) Strafe(speed);
        }

        void IsometricCamera::MouseRotate(const glm::vec2& delta)
        {
            if (m_cameraState == CameraState::ROTATE) {
                Rotate(m_rotationSpeed * delta.x, 0.0f);
            }
        }

        void IsometricCamera::UpdateZoom()
        {
            if (m_eventHandler->GetMouseEvent()->mouse_wheel_scrolled) {
                auto delta = static_cast<GLfloat>(m_eventHandler->GetMouseState()->mouse_scroll_delta);

                if (delta > 0)
                    m_zoom -= 0.125f;
                if (delta < 0)
                    m_zoom += 0.125f;

                if (m_zoom <= 0.125f)
                    m_zoom = 0.125f;
                if (m_zoom >= 8.0f)
                    m_zoom = 8.0f;

                //std::cout << "Camera Zoom: " << (GLfloat)-IsometricCamera::width / 2 * m_zoom << std::endl;
            }
        }

        void IsometricCamera::UpdateProjection()
        {
            m_projection = glm::ortho((GLfloat)-IsometricCamera::width / 2 * m_zoom,
                (GLfloat)IsometricCamera::width / 2 * m_zoom,
                (GLfloat)-IsometricCamera::height / 2 * m_zoom,
                (GLfloat)IsometricCamera::height / 2 * m_zoom,
                -1000.0f, 1000.0f);
        }

        void IsometricCamera::UpdateStates()
        {
            window::KeyboardState* keyState = m_eventHandler->GetKeyboardState();
            window::MouseState* mouseState = m_eventHandler->GetMouseState();

            if (keyState->key_w) m_moveState.up = true;
            else m_moveState.up = false;

            if (keyState->key_s) m_moveState.down = true;
            else m_moveState.down = false;

            if (keyState->key_a) m_moveState.left = true;
            else m_moveState.left = false;

            if (keyState->key_d) m_moveState.right = true;
            else m_moveState.right = false;

            if (mouseState->mouse_button_right) m_cameraState = CameraState::ROTATE;

            else m_cameraState = CameraState::STILL;

            glm::vec2 mousePos = glm::vec2(mouseState->mouse_x, 0.0f);
            m_delta = glm::vec2(m_initialMousePosition.x - mousePos.x, 0.0f);
            m_initialMousePosition = mousePos;
        }
    }
}