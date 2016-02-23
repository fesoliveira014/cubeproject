#ifndef _MESH_H_
#define _MESH_H_

#include "../../Common.h"

#include "../buffer/VertexArray.h"
#include "../buffer/Buffer.h"
#include "../buffer/IndexBuffer.h"
#include "../buffer/VertexAttribute.h"

#include "Vertex.h"

namespace tactical
{
	namespace render
	{
		template<typename VertexType>
		struct Mesh 
		{
			std::vector<VertexType> vertices;
			std::vector<uint> indices;
			IndexBuffer* ibo;
			VertexArray* vao;
		};
	}
}

#endif