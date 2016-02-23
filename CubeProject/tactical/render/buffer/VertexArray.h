#ifndef _VERTEX_ARRAY_H_
#define _VERTEX_ARRAY_H_

#include "../../Common.h"

#include "Buffer.h"
#include "IndexBuffer.h"

namespace tactical
{
	namespace render
	{
		class VertexArray
		{
		public:
			VertexArray();
			~VertexArray();

			void AddBuffer(Buffer* buffer);
			void AddIndexBuffer(IndexBuffer* buffer);

			void Bind();
			void Unbind();

		protected:
			GLuint m_vertexArrayObject;
			std::vector<Buffer*> m_buffers;
			std::vector<IndexBuffer*> m_indexBuffers;
		};
	}
}

#endif