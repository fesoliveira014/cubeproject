#include  "Framebuffer.h"

namespace tactical
{
	namespace render
	{
		Framebuffer::Framebuffer()
		{
			glGenFramebuffers(1, &m_buffer);
			m_isBound = false;
		}

		Framebuffer::~Framebuffer()
		{
			glDeleteFramebuffers(1, &m_buffer);
			m_attachments.clear();
		}

		void Framebuffer::Bind(GLenum target = GL_FRAMEBUFFER)
		{
			glBindFramebuffer(target, m_buffer);
			m_isBound = true;
		}

		void Framebuffer::Unbind(GLenum target = GL_FRAMEBUFFER)
		{
			glBindFramebuffer(target, 0);
			if (target == GL_FRAMEBUFFER) m_isBound = false;
		}

		void Framebuffer::AttachColourbuffer(FramebufferTexture& texture, GLenum colourAttachment)
		{
			m_attachments.push_back(colourAttachment);

			if(!m_isBound) Bind();

			texture.Bind();
			glFramebufferTexture2D(GL_FRAMEBUFFER, colourAttachment, GL_TEXTURE_2D, texture.GetId(), 0);
		}

		void Framebuffer::AttachRenderBuffer(RenderBuffer& buffer, GLenum renderAttachment)
		{
			if (!m_isBound) Bind();

			buffer.Bind();
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, renderAttachment, GL_RENDERBUFFER, buffer.GetId());
		}

		void Framebuffer::Blit(int width, int height, GLenum bufferBit, GLenum filter)
		{
			glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, bufferBit, filter);
		}

		bool Framebuffer::CheckStatus()
		{
			if (!m_isBound) Bind();

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
				std::cout << "Framebuffer not complete!" << std::endl;
				return false;
			}
			return true;
		}

		FramebufferTexture::FramebufferTexture()
		{
			glGenTextures(1, &m_texture);
		}

		FramebufferTexture::FramebufferTexture(GLenum internalFormat, int width, int height, GLenum format, GLenum type)
		{
			glGenTextures(1, &m_texture);
			glBindTexture(GL_TEXTURE_2D, m_texture);
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}

		void FramebufferTexture::SetTexture(GLenum internalFormat, int width, int height, GLenum format, GLenum type)
		{
			glBindTexture(GL_TEXTURE_2D, m_texture);
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}

		FramebufferTexture::~FramebufferTexture()
		{
			glDeleteTextures(1, &m_texture);
		}

		void FramebufferTexture::Bind()
		{
			glBindTexture(GL_TEXTURE_2D, m_texture);
		}

		void FramebufferTexture::Unbind()
		{
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		RenderBuffer::RenderBuffer()
		{
			glGenRenderbuffers(1, &m_buffer);
		}

		RenderBuffer::RenderBuffer(GLenum internalFormat, int width, int height)
		{
			glGenRenderbuffers(1, &m_buffer);
			glBindRenderbuffer(GL_RENDERBUFFER, m_buffer);
			glRenderbufferStorage(GL_RENDERBUFFER, internalFormat , width, height);
		}

		RenderBuffer::~RenderBuffer()
		{
			glDeleteRenderbuffers(1, &m_buffer);
		}

		void RenderBuffer::Bind()
		{
			glBindRenderbuffer(GL_RENDERBUFFER, m_buffer);
		}

		void RenderBuffer::Unbind()
		{
			glBindRenderbuffer(GL_RENDERBUFFER, 0);
		}

		void RenderBuffer::SetRenderBuffer(GLenum internalFormat, int width, int height)
		{
			glBindRenderbuffer(GL_RENDERBUFFER, m_buffer);
			glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, width, height);
		}
	}
}