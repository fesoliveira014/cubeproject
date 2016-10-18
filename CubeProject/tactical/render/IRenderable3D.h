#ifndef _IRENDERABLE_3D_H_
#define _IRENDERABLE_3D_H_

#include "../Common.h"

#include "../math/AABB.h"
#include "structures\Shader.h"

namespace tactical
{
	namespace render
	{
		class IRenderable3D
		{
		public:
			virtual void Draw(Shader& shader) = 0;
			math::AABB GetBoundingBox() { return m_boundingBox; }

		protected:
			math::AABB m_boundingBox;
		};
	}
}

#endif