#ifndef _DRAWABLE_BOX_H_
#define _DRAWABLE_BOX_H_

#include  "../../Common.h"

#include "../../math/AABB.h"
#include "../IRenderable3D.h"
#include "../structures/Vertex.h"
#include "../structures/Mesh.h"
#include  "Geometry.h"

namespace tactical
{
	namespace render
	{
		class DrawableBox : public IRenderable3D
		{
		public:
			DrawableBox(const math::AABB& box);

			void Draw(Shader& shader);

			void SetAABB(const math::AABB& box);
			inline void SetPosition(const glm::vec3& pos) { m_position = pos; }
			glm::vec3 GetPosition() const { return m_position; }

		private:
			void GenerateMesh();
			Mesh<render::Vertex3f3f> m_mesh;

			glm::vec3 m_position;
		};
	}
}

#endif