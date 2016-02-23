#ifndef _BUFFER_H_
#define _BUFFER_H_

#include "../../Common.h"
#include "VertexAttribute.h"

namespace tactical
{
	namespace render
	{
		class Buffer
		{
		public:
			Buffer();
			Buffer(GLvoid* data, GLsizei size, GLuint componentCount, GLuint loc);
			Buffer(GLvoid* data, GLsizei size, const VertexAttribute& attribute);
			Buffer(GLvoid* data, GLsizei size, const std::vector<VertexAttribute>& attributes);
			~Buffer();

			void Bind();
			void Unbind();

			void SetData(GLvoid* data, GLsizei size, GLuint componentCount, GLuint loc);
			void SetSubData(GLvoid* data, GLuint offset, GLsizei size);

			inline const std::vector<VertexAttribute>& GetVertexAttributeArray() { return m_attributes; }
			inline const GLsizei GetStride() { return m_stride; }

		protected:
			GLuint m_buffer;
			GLsizei m_stride;
			std::vector<VertexAttribute> m_attributes;
		};
	}
}

#endif