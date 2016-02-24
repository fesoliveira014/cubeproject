#include "Camera.h"

namespace tactical
{
	namespace render
	{
		Camera::Camera(glm::mat4 projection, glm::vec3 position, glm::vec3 target)
			: m_projection(projection),
			  m_position(position),
			  m_target(target)
		{
			m_up = glm::vec3(0.0f, 1.0f, 0.0f);
			m_right = glm::cross(m_up, m_target);
			m_translation = glm::vec3();
			m_view = glm::mat4(1.0f);

			m_yaw = 0.0f;
			m_pitch = 0.0f;

			m_cameraState = CameraState::STILL;

			m_moveState.forward = false;
			m_moveState.back = false;
			m_moveState.left = false;
			m_moveState.right = false;
			m_moveState.up = false;
			m_moveState.down = false;
		}

		Camera::~Camera()
		{
			m_eventHandler = nullptr;
		}

		void Camera::Rotate(const float yaw, const float pitch, const float roll)
		{
			if (pitch > 0.10f) { m_pitch += .10f; }
			else if (pitch < -0.10f) { m_pitch -= .10f; }
			else { m_pitch += pitch; }

			if (m_pitch > 90.0f) { m_pitch = 90.0f; }
			else if (m_pitch <= -90.0f) { m_pitch = -90.0f; }

			if (yaw > 0.10f) { m_yaw += .10f; }
			else if (yaw < -0.10f) { m_yaw -= .10f; }
			else { m_yaw += yaw; }

			if (m_yaw > 360.0f) { m_yaw -= 360.0f; }
			else if (m_yaw < -360.0f) { m_yaw += 360.0f; }
		}

		void Camera::LinkTo(window::Window& windowHandler)
		{
			m_eventHandler = windowHandler.GetEventHandler();
		}

		/*Ray Camera::CastPickingRay(float viewportX, float viewportY)
		{
			return Ray();
		}*/
	}
}