#include  "Framebuffer.h"

namespace tactical
{
	namespace render
	{
		Framebuffer::Framebuffer()
		{
			glGenFramebuffers(1, &m_buffer);
			m_isBound = false;
			m_width = 0;
			m_height = 0;
		}

		Framebuffer::~Framebuffer()
		{
			glDeleteFramebuffers(1, &m_buffer);
			m_attachments.clear();
		}

		void Framebuffer::Bind(GLenum target)
		{
			glBindFramebuffer(target, m_buffer);
			m_isBound = true;
		}

		void Framebuffer::Unbind(GLenum target)
		{
			glBindFramebuffer(target, 0);
			if (target == GL_FRAMEBUFFER) m_isBound = false;
		}

		void Framebuffer::AttachColourbuffer(FramebufferTexture& texture, GLenum colourAttachment, GLenum drawBuffer, GLenum readBuffer)
		{
			if (m_width == 0 && m_height == 0) {
				m_width = texture.m_width;
				m_height = texture.m_height;
			}
			else if (m_width != texture.m_width && m_height != texture.m_height) {
				LOG_ERROR("Colourbuffer size incompatible with framebuffer!");
				return;
			}

			m_attachments.push_back(colourAttachment);

			if(!m_isBound) Bind();

			texture.Bind();
			glFramebufferTexture2D(GL_FRAMEBUFFER, colourAttachment, GL_TEXTURE_2D, texture.GetId(), 0);
			glDrawBuffer(drawBuffer);
			glReadBuffer(readBuffer);
			Unbind();
		}

		void Framebuffer::AttachRenderBuffer(RenderBuffer& buffer, GLenum renderAttachment)
		{
			if (m_width == 0 && m_height == 0) {
				m_width = buffer.m_width;
				m_height = buffer.m_height;
			}
			else if (m_width != buffer.m_width && m_height != buffer.m_height) {
				LOG_ERROR("Renderbuffer size incompatible with framebuffer!");
				return;
			}

			if (!m_isBound) Bind();

			buffer.Bind();
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, renderAttachment, GL_RENDERBUFFER, buffer.m_handle);
			buffer.Unbind();
		}

		void Framebuffer::Blit(int width, int height, GLenum bufferBit, GLenum filter)
		{
			glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, bufferBit, filter);
		}

		bool Framebuffer::CheckStatus()
		{
			if (!m_isBound) Bind();

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
				LOG_ERROR("Framebuffer not complete!");
				return false;
			}
			Unbind();

			LOG_INFO("Framebuffer OK");

			return true;
		}

		FramebufferTexture::FramebufferTexture()
		{
			m_width = 0;
			m_height = 0;

			glGenTextures(1, &m_handle);
		}

		FramebufferTexture::FramebufferTexture(GLenum internalFormat, int width, int height, GLenum format, GLenum type)
		{
			m_width = width;
			m_height = height;

			glGenTextures(1, &m_handle);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, m_handle);
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_width, m_height, 0, format, type, NULL);
		}

		void FramebufferTexture::SetTexture(GLenum internalFormat, int width, int height, GLenum format, GLenum type)
		{
			glBindTexture(GL_TEXTURE_2D, m_handle);
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, NULL);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		void FramebufferTexture::SetParameterf(GLenum pname, GLfloat param)
		{
			glBindTexture(GL_TEXTURE_2D, m_handle);
			glTexParameterf(GL_TEXTURE_2D, pname, param);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		
		void FramebufferTexture::SetParameteri(GLenum pname, GLint param)
		{
			glBindTexture(GL_TEXTURE_2D, m_handle);
			glTexParameteri(GL_TEXTURE_2D, pname, param);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		void FramebufferTexture::SetParameterfv(GLenum pname, GLfloat* param)
		{
			glBindTexture(GL_TEXTURE_2D, m_handle);
			glTexParameterfv(GL_TEXTURE_2D, pname, param);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		void FramebufferTexture::SetParameteriv(GLenum pname, GLint* param)
		{
			glBindTexture(GL_TEXTURE_2D, m_handle);
			glTexParameteriv(GL_TEXTURE_2D, pname, param);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		FramebufferTexture::~FramebufferTexture()
		{
			glDeleteTextures(1, &m_handle);
		}

		void FramebufferTexture::Bind()
		{
			glBindTexture(GL_TEXTURE_2D, m_handle);
		}

		void FramebufferTexture::Unbind()
		{
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		RenderBuffer::RenderBuffer() 
		{
			m_width = 0;
			m_height = 0;

			glGenRenderbuffers(1, &m_handle);
		}

		RenderBuffer::RenderBuffer(GLenum internalFormat, int width, int height)
		{
			m_width = width;
			m_height = height;

			glGenRenderbuffers(1, &m_handle);
			glBindRenderbuffer(GL_RENDERBUFFER, m_handle);
			glRenderbufferStorage(GL_RENDERBUFFER, internalFormat , width, height);
		}

		RenderBuffer::~RenderBuffer()
		{
			glDeleteRenderbuffers(1, &m_handle);
		}

		void RenderBuffer::Bind()
		{
			glBindRenderbuffer(GL_RENDERBUFFER, m_handle);
		}

		void RenderBuffer::Unbind()
		{
			glBindRenderbuffer(GL_RENDERBUFFER, 0);
		}

		void RenderBuffer::SetRenderBuffer(GLenum internalFormat, int width, int height)
		{
			glBindRenderbuffer(GL_RENDERBUFFER, m_handle);
			glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, width, height);
		}
	}
}