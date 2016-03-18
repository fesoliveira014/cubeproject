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

			bool Contains(const glm::vec3& point);

			void SetMin(const glm::vec3& min);
			void SetMax(const glm::vec3& max);

			inline const glm::vec3 GetMin() const { return m_min; }
			inline const glm::vec3 GetMax() const { return m_max; }
			inline const glm::vec3 GetCenter() const { return m_center; }
			inline const glm::vec3 GetSize() const { return m_dim; }

		private:
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