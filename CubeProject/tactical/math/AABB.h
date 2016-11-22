#ifndef _AABB_H_
#define _AABB_H_

#include "../Common.h"

namespace tactical
{
	namespace math
	{
		class AABB
		{
		public:
			AABB();
			AABB(const glm::vec3& min, const glm::vec3& max);
			AABB(const AABB& box);

			bool Intersects(const glm::vec3& vector);
			bool Intersects(AABB& box);

			void CalculateAABB(const glm::mat4& model)
			{
				m_min = glm::vec3(model * glm::vec4(m_min, 0.0f));
				m_max = glm::vec3(model * glm::vec4(m_max, 0.0f));
				UpdateAttributes();
			}

			template <typename Vertex>
			void CalculateAABB(std::vector<Vertex> vertices)
			{
				m_max = m_min = glm::vec3(0.0f);

				for (uint i = 0; i < vertices.size(); ++i) {
					if (vertices[i].m_position.x > m_max.x) m_max.x = vertices[i].m_position.x;
					if (vertices[i].m_position.y > m_max.y) m_max.y = vertices[i].m_position.y;
					if (vertices[i].m_position.z > m_max.z) m_max.z = vertices[i].m_position.z;
					if (vertices[i].m_position.x < m_min.x) m_min.x = vertices[i].m_position.x;
					if (vertices[i].m_position.y < m_min.y) m_min.y = vertices[i].m_position.y;
					if (vertices[i].m_position.z < m_min.z) m_min.z = vertices[i].m_position.z;

				}
				
				UpdateAttributes();
			}

			template <typename Vertex>
			void CalculateAABB(std::vector<Vertex> vertices, const glm::mat4& model, const glm::vec3& pos)
			{
				m_max = m_min = glm::vec3(0.0f);
				glm::vec3 position;

				for (uint i = 0; i < vertices.size(); ++i) {
					position = glm::vec3(model * glm::vec4(vertices[i].m_position, 1.0f)) - pos;

					if (position.x > m_max.x) m_max.x = position.x;
					if (position.y > m_max.y) m_max.y = position.y;
					if (position.z > m_max.z) m_max.z = position.z;
					if (position.x < m_min.x) m_min.x = position.x;
					if (position.y < m_min.y) m_min.y = position.y;
					if (position.z < m_min.z) m_min.z = position.z;
				}

				UpdateAttributes();
			}

			bool Contains(const glm::vec3& point);

			void SetMin(const glm::vec3& min);
			void SetMax(const glm::vec3& max);

			inline const glm::vec3 GetMin() const { return m_min; }
			inline const glm::vec3 GetMax() const { return m_max; }
			inline const glm::vec3 GetCenter() const { return m_center; }
			inline const glm::vec3 GetSize() const { return m_dim; }

		protected:
			void UpdateAttributes();

			glm::vec3 m_min;
			glm::vec3 m_max;
			glm::vec3 m_center;
			glm::vec3 m_dim;

		};

		inline void AABB::SetMin(const glm::vec3& min)
		{
			m_min = min;
		}

		inline void AABB::SetMax(const glm::vec3& max)
		{
			m_max = max;
		}
	}
}

#endif