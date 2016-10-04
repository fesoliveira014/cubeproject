#pragma once

#include "Camera.h"

namespace tactical
{
  namespace render
  {
    class IsometricCamera : public Camera
    {
    public:
      IsometricCamera(const glm::mat4& projection, const glm::vec3& position, const glm::vec3& target);
      ~IsometricCamera();

      void Update() override;
    private:
      void Move(float speed);
      void UpdateStates();

      float m_speed;
      glm::vec3 m_worldUp;
    };
  }
}