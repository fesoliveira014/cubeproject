#ifndef _PRISM_H_
#define _PRISM_H_

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
		class Prism : public IEntity3D<render::Vertex3f3f4f>
		{
		public:
			Prism(const glm::vec3& pos);
			~Prism();

			void Draw(render::Shader& shader);
		private:
			void CalculateTransform();
			void GenerateMesh();
		};
	}
}

#endif