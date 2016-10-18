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

		void Camera::Update(float deltaTime)
		{

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

        void Camera::UpdateBasis()
        {
            glm::mat4 rotationMatrix = glm::yawPitchRoll(m_yaw, m_pitch, 0.0f);
            // calculate forward, up, right and target vectors
            m_forward = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f)));
            m_target = m_position + m_forward;

            m_up = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)));
            m_right = glm::normalize(glm::cross(m_forward, m_up));

            // use position, target and up vectors to calculate view matrix
            m_view = glm::lookAt(m_position, m_target, m_up);
        }

        math::Ray& Camera::CastPickingRay(const glm::vec3& origin, const glm::vec2& mouse, const glm::vec2& viewport)
        {
            float x = (2.0f * mouse.x) / viewport.x - 1.0f;
            float y = 1.0f - (2.0f * mouse.y) / viewport.y;

            glm::vec4 clip = glm::vec4(x, y, -1.0f, 1.0f);

            glm::vec4 eye = glm::inverse(m_projection) * clip;
            eye = glm::vec4(eye.x, eye.y, -1.0f, 0.0f);

            glm::vec3 world = glm::vec3(glm::inverse(m_view) * eye);
            world = glm::normalize(world);

            math::Ray ray(origin, world);
            return std::move(ray);
        }
    }
}