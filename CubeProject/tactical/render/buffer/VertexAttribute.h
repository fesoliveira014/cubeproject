#ifndef _VERTEX_ATTRIBUTE_H_
#define _VERTEX_ATTRIBUTE_H_

#include "../../Common.h"

namespace tactical
{
	namespace render
	{
		struct VertexAttribute
		{
			GLuint loc;
			uint numComponents;
			GLType type;

			VertexAttribute(GLuint l, int n, GLType t) :
				loc(l), numComponents(n), type(t)
			{

			}
			
		};
	}
}

#endif