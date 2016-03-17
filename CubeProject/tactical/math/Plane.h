#ifndef _PLANE_H_
#define _PLANE_H_

#include "../Common.h"

#include "AABB.h"

namespace tactical
{
	namespace math
	{
		class Plane
		{
		public:
			enum class PlaneSide
			{
				OUTSIDE = 0, INSIDE, INTERSECTING
			};

			Plane();
			Plane(const Plane& plane);
			Plane(const glm::vec3& normal, float scalar);
			Plane(float x, float y, float z, float scalar);
			Plane(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);
			~Plane();

			PlaneSide Intersects(const AABB& box);
			float DistanceFromPoint(const glm::vec3& point);

			void Set(const glm::vec3& normal, float scalar);
			void SetAndNormalize(const glm::vec3& normal, float scalar);
			void SetNormal(const glm::vec3& normal);
			void SetScalar(float scalar);

			inline glm::vec3 GetNormal() const { return m_normal; }
			inline float GetScalar() const { return m_scalar; }

		private:
			glm::vec3 m_normal;
			float m_scalar;
		};
	}
}

#endif