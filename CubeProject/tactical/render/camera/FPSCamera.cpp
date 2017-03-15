#include "FPSCamera.h"

namespace tactical
{
    namespace render
    {
        FPSCamera::FPSCamera(const glm::mat4& projection, const glm::vec3& position, const glm::vec3& target)
            : Camera(projection, position, target)
        {
            m_speed = 20.0f;
            m_panSpeed = 0.01f;
            m_rotationSpeed = 0.005f;

            m_distance = glm::distance(m_position, m_target);
        }

        FPSCamera::~FPSCamera()
        {
        }

        void FPSCamera::Update(float deltaTime)
        {
            // update state variables and deltas
            UpdateStates();

            float velocity = m_speed * deltaTime;
            Move(velocity);

            MouseRotate(m_delta);

            m_position += m_translation;
            m_translation = glm::vec3(0.0f);

            // update orthonormal basis
            UpdateBasis();
        }

        void FPSCamera::Move(float speed)
        {            
            if (m_moveState.forward) Walk(speed);
            if (m_moveState.back) Walk(-speed);
            if (m_moveState.left) Strafe(-speed);
            if (m_moveState.right) Strafe(speed);
            if (m_moveState.up) Lift(speed);
            if (m_moveState.down) Lift(-speed);

            if (m_cameraState == CameraState::PAN) {
                Walk(speed * m_delta.y);
                Strafe(speed * m_delta.x);
            }
        }

        void FPSCamera::MouseRotate(const glm::vec2& delta)
        {
            if (m_cameraState == CameraState::ROTATE)
                Rotate(m_rotationSpeed * delta.x, m_rotationSpeed * delta.y);
        }

        void FPSCamera::UpdateStates()
        {            
            window::KeyboardState* keyState = m_eventHandler->GetKeyboardState();
            window::MouseState* mouseState = m_eventHandler->GetMouseState();

            if (keyState->key_w) m_moveState.forward = true;
            else m_moveState.forward = false;

            if (keyState->key_s) m_moveState.back = true;
            else m_moveState.back = false;

            if (keyState->key_a) m_moveState.left = true;
            else m_moveState.left = false;

            if (keyState->key_d) m_moveState.right = true;
            else m_moveState.right = false;

            if (keyState->key_space) m_moveState.up = true;
            else m_moveState.up = false;

            if (keyState->key_c) m_moveState.down = true;
            else m_moveState.down = false;

            if (mouseState->mouse_button_right) m_cameraState = CameraState::ROTATE;
            //else if (mouseState->mouse_button_left) m_cameraState = CameraState::PAN;
            else m_cameraState = CameraState::STILL;

            glm::vec2 mousePos = glm::vec2(mouseState->mouse_x, mouseState->mouse_y);
            m_delta = glm::vec2(m_initialMousePosition.x - mousePos.x, mousePos.y - m_initialMousePosition.y);
            m_initialMousePosition = mousePos;
        }

		math::Ray FPSCamera::CastPickingRay(const glm::vec2& mouse, const glm::vec2& viewport)
		{
			glm::vec3 origin = m_position;

			// Screen space to Clip space
			float x = (2.0f * mouse.x) / viewport.x - 1.0f;
			float y = 1.0f - (2.0f * mouse.y) / viewport.y;

			glm::vec4 clip = glm::vec4(x, y, -1.0f, 1.0f);

			// Clip space to  View space
			glm::vec4 eye = glm::inverse(m_projection) * clip;
			eye = glm::vec4(eye.x, eye.y, -1.0f, 0.0f);

			// View space to World space
			glm::vec3 world = glm::vec3(glm::inverse(m_view) * eye);
			world = glm::normalize(world);

			return math::Ray(origin, world);
		}
    }
}