#ifndef _SPHERE_H_
#define _SPHERE_H_

#include "../Common.h"

#include "../render/structures/Shader.h"
#include "../render/structures/Mesh.h"
#include "../render/structures/Vertex.h"
#include "../render/geometry/Geometry.h"

#include "interface\IEntity3D.h"

namespace tactical
{
	namespace entity
	{
		class Sphere : public IEntity3D<render::Vertex3f3f4f>
		{
		public:
			Sphere(const glm::vec3 & center, const glm::vec3 & radius);
			//Sphere(const Sphere & sphere);
			~Sphere();

		private:
			void GenerateMesh();
			void CalculateTransform();

			glm::vec3 m_radius;
		};
	}
}

#endif