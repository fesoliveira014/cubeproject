#pragma once

#include "Camera.h"

namespace tactical
{
    namespace render
    {
        constexpr static float ISOMETRIC_WIDTH = 64.0f;
        constexpr static float ISOMETRIC_HEIGHT = 64.0f;
        constexpr static float ISOMETRIC_DISTANCE = 128.0f; // this has to be a function of the world height (maybe 2 * worldHeight?)

        class IsometricCamera : public Camera
        {
        public:
            IsometricCamera(const glm::mat4& projection,
                const glm::vec3& position = glm::vec3(0.0f, 0.0f, 0.0f),
                const glm::vec3& target = glm::vec3(0.0f, 0.0f, 0.0f));
            ~IsometricCamera();

            void Update(float deltaTime) override;

			math::Ray CastPickingRay(const glm::vec2& mouse, const glm::vec2& viewport) override;

        private:
            void Initialize();
            glm::vec3 GetPosition() override;
            void Move(float speed);
            void MouseRotate(const glm::vec2& delta);
            void UpdateZoom();
            void UpdateProjection();
            void UpdateStates();

            glm::vec2 m_initialMousePosition, m_delta;
            glm::vec3 m_initialTarget, m_initialRotation;

            float m_zoom;
            float m_speed, m_rotationSpeed;
        };
    }
}