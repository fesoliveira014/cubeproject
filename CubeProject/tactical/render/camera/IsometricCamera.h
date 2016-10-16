#pragma once

#include "Camera.h"

namespace tactical
{
    namespace render
    {
        class IsometricCamera : public Camera
        {
        public:
            IsometricCamera(const glm::mat4& projection = glm::ortho((GLfloat)-IsometricCamera::width / 2,
                (GLfloat)IsometricCamera::width / 2,
                (GLfloat)-IsometricCamera::height / 2,
                (GLfloat)IsometricCamera::height / 2,
                -1000.0f, 1000.0f),
                const glm::vec3& position = glm::vec3(0.0f, 0.0f, 0.0f),
                const glm::vec3& target = glm::vec3(0.0f, 0.0f, 0.0f));
            ~IsometricCamera();

            void Update(float deltaTime) override;
        private:
            constexpr static float width = 64.0f;
            constexpr static float height = 64.0f;

            void Initialize();
            void Move(float speed);
            void MouseRotate(const glm::vec2& delta);
            void UpdateZoom();
            void IsometricCamera::UpdateProjection();
            void UpdateStates();

            glm::vec2 m_initialMousePosition, m_delta;
            glm::vec3 m_initialTarget, m_initialRotation;

            float m_zoom;
            float m_speed, m_rotationSpeed;
        };
    }
}