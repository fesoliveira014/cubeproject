#ifndef _SHPERE_H_
#define _SHPERE_H_

#include "../Common.h"

namespace tactical
{
	namespace math
	{
		struct Sphere
		{
			Sphere() : center(glm::vec3(0.0f, 0.0f, 0.0f)), radius(0.0f) { }
			Sphere(const glm::vec3& c, float r) : center(c), radius(r) { }
			
			glm::vec3 center;
			float radius;
		};
	}
}

#endif