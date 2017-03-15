#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "../../Common.h"
#include "../../window/Window.h"
#include "../../math/Ray.h"

#include "../../utils/Logger.h"

namespace tactical
{
    namespace render
    {
        class Camera
        {
        public:
            enum class CameraState
            {
                STILL = 0,
                ROTATE,
                PAN,
            };

            struct MoveState
            {
                bool forward;
                bool back;
                bool left;
                bool right;
                bool up;
                bool down;
            };

            Camera(glm::mat4 projection, glm::vec3 position, glm::vec3 target);
            virtual ~Camera();

            virtual void Update(float deltaTime);

            void Rotate(const float yaw, const float pitch, const float roll = 0.0f);

            inline void Translate(const glm::vec3& translation) { m_translation += translation; }
            inline void Translate(float x, float y, float z) { m_translation += glm::vec3(x, y, z); }

            virtual math::Ray CastPickingRay(const glm::vec2& mouse, const glm::vec2& viewport) = 0;

            inline const glm::mat4 GetProjectionMatrix() const { return m_projection; }
            inline const glm::mat4 GetViewMatrix() const { return m_view; }

            inline void SetProjectionMatrix(const glm::mat4& projection) { m_projection = projection; }

            inline void SetPosition(const glm::vec3& position) { m_position = position; }
            inline void SetPosition(float x, float y, float z) { m_position = glm::vec3(x, y, z); }
            virtual inline glm::vec3 GetPosition() { return m_position; }

            inline const glm::vec3 GetUpDirection() const { return m_up; }
            inline const glm::vec3 GetRightDirection() const { return m_right; }
            inline const glm::vec3 GetForwardDirection() const { return m_forward; }
            inline const glm::vec3 GetTarget() const { return m_target; }

            void LinkTo(window::Window& windowHandler);

        protected:
            void UpdateBasis();

            inline void Walk(const float distance) { m_translation += m_forward * distance; }
            inline void Strafe(const float distance) { m_translation += m_right * distance; }
            inline void Lift(const float distance) { m_translation += m_up * distance; }

            glm::vec3 m_position;
            glm::vec3 m_target;
            glm::vec3 m_up, m_right, m_forward;
            glm::vec3 m_translation;

            glm::mat4 m_projection;
            glm::mat4 m_view;

            float m_yaw, m_pitch;

            CameraState m_cameraState;
            MoveState m_moveState;

            std::shared_ptr<window::EventHandler> m_eventHandler;
        };
    }
}

#endif
