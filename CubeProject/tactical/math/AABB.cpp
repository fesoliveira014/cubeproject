#include "AABB.h"

namespace tactical
{
	namespace math
	{
		AABB::AABB() : m_min(glm::vec3(0.0f)), m_max(glm::vec3(0.0f)), m_center(glm::vec3(0.0f))
		{
			m_dim = glm::vec3(0.0f);
		}

		AABB::AABB(const glm::vec3& min, const glm::vec3& max) : m_min(min), m_max(max)
		{
			UpdateAttributes();
		}

		void AABB::UpdateAttributes()
		{
			m_center = (m_max + m_min) / 2.0f;

			m_dim.x = abs(m_max.x - m_min.x);
			m_dim.y = abs(m_max.y - m_min.y);
			m_dim.z = abs(m_max.z - m_min.z);
		}

		bool AABB::Intersects(const glm::vec3& vector)
		{
			return (vector.x >= m_min.x && vector.x <= m_max.x &&
					vector.y >= m_min.y && vector.y <= m_max.y &&
					vector.z >= m_min.z && vector.z <= m_max.z);
		}

		bool AABB::Intersects(AABB& box)
		{
			if (m_max.x < box.m_min.x) return false;
			if (m_max.y < box.m_min.y) return false;
			if (m_max.z < box.m_min.z) return false;
			if (m_min.x > box.m_max.x) return false;
			if (m_min.y > box.m_max.y) return false;
			if (m_min.z > box.m_max.z) return false;

			return true;
		}

		bool AABB::Contains(const glm::vec3& point)
		{
			return (point.x >= m_min.x && point.x <= m_max.x &&
					point.y >= m_min.y && point.y <= m_max.y &&
					point.z >= m_min.z && point.z <= m_max.z);
		}
	}
}