#ifndef _DRAWABLE_LINE_H_
#define _DRAWABLE_LINE_H_

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
		class DrawableLine : public IRenderable3D
		{
		public:
			DrawableLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

			void Draw(Shader& shader);

			inline void SetStart(const glm::vec3& pos) { m_start = pos; }
			inline void SetEnd(const glm::vec3& pos) { m_end = pos; }

			inline void Set(const glm::vec3& start, const glm::vec3& end) 
			{
				m_start = start;
				m_end = end;

				GenerateMesh();
			}

			glm::vec3 GetStart() const { return m_start; }
			glm::vec3 GetEnd() const { return m_end; }

		private:
			void GenerateMesh();
			Mesh<render::Vertex3f4f> m_mesh;

			glm::vec3 m_start;
			glm::vec3 m_end;
			glm::vec4 m_color;
		};
	}
}

#endif