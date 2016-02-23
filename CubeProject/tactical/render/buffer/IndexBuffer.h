#ifndef _INDEX_BUFFER_H_
#define _INDEX_BUFFER_H_

#include "../../Common.h"

namespace tactical
{
	namespace render
	{
		class IndexBuffer
		{
		public:
			IndexBuffer(GLuint* data, GLsizei count);
			~IndexBuffer();

			void Bind();
			void Unbind();

			inline GLuint GetCount() const { return m_count; }
		protected:
			GLuint m_buffer;
			GLuint m_count;
		};
	}
}

#endif