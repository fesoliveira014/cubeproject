#ifndef _IRENDERABLE_3D_H_
#define _IRENDERABLE_3D_H_

#include "../Common.h"

#include "structures\Shader.h"

namespace tactical
{
	namespace render
	{
		class IRenderable3D
		{
		public:
			virtual void Draw(Shader& shader) = 0;
		};
	}
}

#endif