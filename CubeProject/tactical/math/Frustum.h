#ifndef _FRUSTUM_H_
#define _FRUSTUM_H_

#include "../Common.h"

#include "Sphere.h"
#include "Plane.h"
#include "AABB.h"

namespace tactical
{
	namespace math
	{
		class Frustum
		{
		public:
			Frustum();
			~Frustum();

			void Update(glm::mat4 clipMatrix);
			void Update(glm::mat4 projection, glm::mat4 view);

			bool Contains(const glm::vec3& point);
			bool Contains(const Sphere& sphere);
			bool Contains(const AABB& box);
		private:
			Plane m_planes[6];
		};
	}
}

#endif