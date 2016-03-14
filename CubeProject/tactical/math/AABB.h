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

			bool Intersects(const glm::vec3& vector);
			bool Intersects(AABB& box);

			bool Contains(const glm::vec3& point);

			void SetMin(const glm::vec3& min);
			void SetMax(const glm::vec3& max);

			inline const glm::vec3 GetMin() const { return m_min; }
			inline const glm::vec3 GetMax() const { return m_max; }
			inline const glm::vec3 GetCenter() const { return m_center; }
			inline const glm::vec3 GetSize() const { return m_dim; }

			inline glm::vec3 GetCornerP(const glm::vec3& vec) const;
			inline glm::vec3 GetCornerN(const glm::vec3& vec) const;

		private:
			void UpdateAttributes();

			glm::vec3 m_min;
			glm::vec3 m_max;
			glm::vec3 m_center;
			glm::vec3 m_dim;

		};

		inline glm::vec3 AABB::GetCornerP(const glm::vec3& vec) const
		{
			glm::vec3 result = m_min;

			if (vec.x > 0)
				result.x += m_max.x;

			if (vec.y > 0)
				result.y += m_max.y;

			if (vec.z > 0)
				result.z += m_max.z;

			return std::move(result);
		}

		inline glm::vec3 AABB::GetCornerN(const glm::vec3& vec) const
		{
			glm::vec3 result = m_min;

			if (vec.x < 0)
				result.x += m_max.x;

			if (vec.y < 0)
				result.y += m_max.y;

			if (vec.z < 0)
				result.z += m_max.z;

			return std::move(result);
		}

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