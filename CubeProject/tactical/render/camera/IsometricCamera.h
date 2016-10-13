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

			void Update(float deltaTime) override;
		private:
			void Move(float speed);
			void MouseRotate(const glm::vec2& delta);
			void UpdateStates();

			glm::vec2 m_initialMousePosition, m_delta;
			glm::vec3 m_initialTarget, m_initialRotation;

			float m_speed, m_rotationSpeed;
		};
	}
}