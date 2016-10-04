#include "IsometricCamera.h"

namespace tactical
{
  namespace render
  {
    IsometricCamera::IsometricCamera(const glm::mat4& projection, const glm::vec3& position, const glm::vec3& target)
      : Camera(projection, position, target)
    {
      m_yaw = -45.0f;
      m_pitch = -35.264f;
      m_speed = 2.5f;
      m_worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    }

    IsometricCamera::~IsometricCamera()
    {
    }

    void IsometricCamera::Update(float deltaTime)
    {
      UpdateStates();

      float velocity = m_speed * deltaTime;
      Move(velocity);

      // get rotation matrix from yaw and pitch and execute translation
      glm::mat4 rotationMatrix = glm::yawPitchRoll(m_yaw, m_pitch, 0.0f);
      m_position += m_translation;

      m_translation = glm::vec3(0.0f);

      glm::vec3 front;
      front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
      front.y = sin(glm::radians(m_pitch));
      front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
      m_forward = glm::normalize(front);
      m_target = m_position + m_forward;

      m_right = glm::normalize(glm::cross(m_forward, m_worldUp));
      m_up = glm::normalize(glm::cross(m_right, m_forward));

      m_view = glm::lookAt(m_position, m_target, m_up);
    }

    void IsometricCamera::Move(float speed)
    {
      if (m_moveState.up) Lift(speed);
      if (m_moveState.down) Lift(-speed);
      if (m_moveState.left) Strafe(-speed);
      if (m_moveState.right) Strafe(speed);
    }

    void IsometricCamera::UpdateStates()
    {
      window::KeyboardState* keyState = m_eventHandler->GetKeyboardState();

      if (keyState->key_w) m_moveState.up = true;
      else m_moveState.up = false;

      if (keyState->key_s) m_moveState.down = true;
      else m_moveState.down = false;

      if (keyState->key_a) m_moveState.left = true;
      else m_moveState.left = false;

      if (keyState->key_d) m_moveState.right = true;
      else m_moveState.right = false;
    }
  }
}