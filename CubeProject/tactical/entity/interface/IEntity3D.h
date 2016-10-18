#ifndef _IENTITY3D_H_
#define _IENTITY3D_H_

#include "../../Common.h"
#include "../../render/structures/Shader.h"
#include "../../render/structures/Mesh.h"
#include "../../render/IRenderable3D.h" 

namespace tactical {
	namespace entity {
		template <typename V>
		class IEntity3D : public render::IRenderable3D
		{
		public:
			virtual void Draw(render::Shader& shader) = 0;

			void Rotate(float angle, const glm::vec3 &axis);

			void RotateX(float angle);
			void RotateY(float angle);
			void RotateZ(float angle);

			void Translate(const glm::vec3 &pos);
			void Scale(const glm::vec3 &scale);

			inline const glm::mat4 GetModelMatrix() { return m_model; }

			inline const glm::vec3 GetPosition() { return m_position; }
			inline void SetPosition(const glm::vec3 &pos) { m_position = pos; }

		protected:
			glm::vec3 m_position;
			glm::mat4 m_model;

			render::Mesh<V> m_mesh;
		};
	}
}

#include "IEntity3D.impl" 



#endif