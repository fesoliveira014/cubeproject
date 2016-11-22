#ifndef _DRAWABLE_BOX_H_
#define _DRAWABLE_BOX_H_

#include  "../Common.h"

#include "../math/AABB.h"
#include  "../render/structures/Vertex.h"
#include "interface/IEntity3D.h"
#include  "../render/geometry/Geometry.h"

namespace tactical
{
	namespace entity
	{
		class DrawableBox : public IEntity3D<render::Vertex3f3f>
		{
		public:
			DrawableBox(const math::AABB& box);

			void Draw(render::Shader& shader);

			void SetAABB(const math::AABB& box);

		private:
			void CalculateTransform();
			void GenerateMesh();

		};
	}
}

#endif