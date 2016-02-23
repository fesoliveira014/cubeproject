#include "VertexArray.h"

namespace tactical
{
	namespace render
	{
		VertexArray::VertexArray()
		{
			glGenVertexArrays(1, &m_vertexArrayObject);
		}

		VertexArray::~VertexArray()
		{
			for (auto buffer : m_buffers)
			{
				delete buffer;
			}

			for (auto buffer : m_indexBuffers)
			{
				delete buffer;
			}

			glDeleteVertexArrays(1, &m_vertexArrayObject);
		}

		void VertexArray::AddBuffer(Buffer* buffer)
		{
			uint offset = 0;
			m_buffers.push_back(buffer);

			Bind();
			buffer->Bind();

			for (VertexAttribute attribute : buffer->GetVertexAttributeArray()) {
				glVertexAttribPointer(attribute.loc, attribute.numComponents,
					static_cast<GLenum>(attribute.type), GL_FALSE, buffer->GetStride(), (const GLvoid*) offset);
				glEnableVertexAttribArray(attribute.loc);

				offset += attribute.numComponents * GLTypeSize(attribute.type);
			}

			buffer->Unbind();
			Unbind();
		}

		void VertexArray::AddIndexBuffer(IndexBuffer* buffer)
		{
			m_indexBuffers.push_back(buffer);

			Bind();
			buffer->Bind();
			Unbind();
		}

		void VertexArray::Bind()
		{
			glBindVertexArray(m_vertexArrayObject);
		}

		void VertexArray::Unbind()
		{
			glBindVertexArray(0);
		}
	}
}