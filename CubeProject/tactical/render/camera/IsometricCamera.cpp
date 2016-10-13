#include "IsometricCamera.h"

namespace tactical
{
	namespace render
	{
		IsometricCamera::IsometricCamera(const glm::mat4& projection, const glm::vec3& position, const glm::vec3& target)
			: Camera(projection, position, target)
		{
			m_pitch = glm::radians(45.0f);
			// needs to yaw -90 degrees because of the quadradnt the scene is being drawed,
			// plus -arctan(1/sqr(2)) =~ -35.264 degrees
			m_yaw = glm::radians(-125.264f);
			m_speed = 6.0f;
			m_rotationSpeed = 0.003f;
		}

		IsometricCamera::~IsometricCamera()
		{
		}

		void IsometricCamera::Update(float deltaTime)
		{
			UpdateStates();

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