#include "FPSCamera.h"

namespace tactical
{
	namespace render
	{
		FPSCamera::FPSCamera(const glm::mat4& projection, const glm::vec3& position, const glm::vec3& target)
			: Camera(projection, position, target)
		{
			m_speed = 0.025f;
			m_panSpeed = 0.01f;
			m_rotationSpeed = 0.005f;

			m_distance = glm::distance(m_position, m_target);
		}

		FPSCamera::~FPSCamera()
		{
			
		}

		void FPSCamera::Update()
		{
			// update state variables and deltas
			UpdateStates();
			Move(m_speed);
			MouseRotate(m_delta);

			// get rotation matrix from yaw and pitch and execute translation
			glm::mat4 rotationMatrix = glm::yawPitchRoll(m_yaw, m_pitch, 0.0f);
			m_position += m_translation;

			m_translation = glm::vec3(0.0f);

			// calculate forward, up, right and target vectors
			m_forward = glm::vec3(rotationMatrix * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f));
			m_target = m_position + m_forward;

			m_up = glm::vec3(rotationMatrix * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
			m_right = glm::cross(m_up, m_forward);

			// use position, target and up vectors to calculate view matrix
			m_view = glm::lookAt(m_position, m_target, m_up);
		}

		void FPSCamera::Move(float speed)
		{
			if (m_moveState.forward) Walk(speed);
			if (m_moveState.back) Walk(-speed);
			if (m_moveState.left) Strafe(speed);
			if (m_moveState.right) Strafe(-speed);
			if (m_moveState.up) Lift(speed);
			if (m_moveState.down) Lift(-speed);

			if (m_cameraState == CameraState::PAN){
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
			else m_moveState.up= false;

			if (keyState->key_c) m_moveState.down = true;
			else m_moveState.down = false;

			if (mouseState->mouse_button_right) m_cameraState = CameraState::ROTATE;
			//else if (mouseState->mouse_button_left) m_cameraState = CameraState::PAN;
			else m_cameraState = CameraState::STILL;
			
			glm::vec2 mousePos = glm::vec2(mouseState->mouse_x, mouseState->mouse_y);
			m_delta = glm::vec2(m_initialMousePosition.x - mousePos.x, mousePos.y - m_initialMousePosition.y);
			m_initialMousePosition = mousePos;
		}
	}
}