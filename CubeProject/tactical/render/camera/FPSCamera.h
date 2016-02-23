#include "Camera.h"

namespace tactical
{
	namespace render
	{
		class FPSCamera : public Camera
		{
		public:
			FPSCamera(const glm::mat4& projection, const glm::vec3& position, const glm::vec3& target);
			~FPSCamera();

			void Update() override;

		private:
			//void MousePan(const glm::vec2& delta);
			void MouseRotate(const glm::vec2& delta);
			void Move(float speed);

			// need to be moved to an external class later on
			void UpdateStates();

			glm::vec2 m_initialMousePosition, m_delta;
			glm::vec3 m_initialTarget, m_initialRotation;

			float m_distance;
			float m_speed, m_panSpeed, m_rotationSpeed;

			
		};
	}
}