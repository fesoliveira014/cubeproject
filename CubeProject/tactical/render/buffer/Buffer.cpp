#include "Buffer.h"

namespace tactical
{
	namespace render
	{
		Buffer::Buffer()
		{
			glGenBuffers(1, &m_buffer);
			m_stride = 0;
		}

		Buffer::Buffer(GLvoid* data, GLsizei size, GLuint componentCount, GLuint loc)
		{
			glGenBuffers(1, &m_buffer);
			glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
			glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			m_attributes.push_back(VertexAttribute(loc, componentCount, GLType::FLOAT));
			m_stride = componentCount * GLTypeSize(GLType::FLOAT);
		}

		Buffer::Buffer(GLvoid* data, GLsizei size, const VertexAttribute& attribute)
		{
			glGenBuffers(1, &m_buffer);
			glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
			glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			m_attributes.push_back(attribute);
			m_stride = attribute.numComponents * GLTypeSize(attribute.type);
		}

		Buffer::Buffer(GLvoid* data, GLsizei size, const std::vector<VertexAttribute>& attributes)
		{
			glGenBuffers(1, &m_buffer);
			glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
			glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			m_attributes = attributes;

			m_stride = 0;
			for (VertexAttribute attribute : m_attributes)
				m_stride += attribute.numComponents * GLTypeSize(attribute.type);
		}

		void Buffer::SetData(GLvoid* data, GLsizei size, GLuint componentCount, GLuint loc)
		{
			glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
			glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			m_attributes.push_back(VertexAttribute(loc, componentCount, GLType::FLOAT));
			m_stride = componentCount;
		}

		void Buffer::SetSubData(GLvoid* data, GLuint offset, GLsizei size)
		{
			glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
			glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

		Buffer::~Buffer()
		{
			glDeleteBuffers(1, &m_buffer);
		}

		void Buffer::Bind()
		{
			glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
		}

		void Buffer::Unbind()
		{
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
	}
}