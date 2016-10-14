#ifndef _FRAMEBUFFER_H_
#define _FRAMEBUFFER_H_

#include "../../utils/Logger.h"
#include "../../Common.h"

namespace tactical
{
	namespace render
	{
		class FramebufferTexture
		{
			friend class Framebuffer;

		public:
			FramebufferTexture();
			FramebufferTexture(GLenum internalFormat, int width, int height, GLenum format, GLenum type);
			~FramebufferTexture();

			void Bind();
			void Unbind();

			void SetTexture(GLenum internalFormat, int width, int height, GLenum format, GLenum type);

			void SetParameterf(GLenum pname, GLfloat param);
			void SetParameteri(GLenum pname, GLint param);
			void SetParameterfv(GLenum pname, GLfloat* param);
			void SetParameteriv(GLenum pname, GLint* param);

			GLuint GetId() { return m_texture; }

		private:
			GLuint m_texture;
			int m_width, m_height;
		};

		class RenderBuffer
		{
			friend class Framebuffer;

		public:
			RenderBuffer();
			RenderBuffer(GLenum internalFormat, int width, int height);
			~RenderBuffer();

			void Bind();
			void Unbind();

			void SetRenderBuffer(GLenum internalFormat, int width, int height);

			GLuint GetId() { return m_buffer; }
			
		private:
			GLuint m_buffer;
			int m_width, m_height;
		};

		class Framebuffer
		{
		public:
			Framebuffer();
			~Framebuffer();

			void Bind(GLenum target = GL_FRAMEBUFFER);
			void Unbind(GLenum target = GL_FRAMEBUFFER);

			void AttachColourbuffer(FramebufferTexture& texture, GLenum colourAttachment, GLenum drawBuffer = GL_NONE, GLenum readBuffer = GL_NONE);
			void AttachRenderBuffer(RenderBuffer& buffer, GLenum renderAttachment);

			void Blit(int width, int height, GLenum bufferBit = GL_DEPTH_BUFFER_BIT, GLenum filter = GL_NEAREST);

			bool CheckStatus();

			const int GetWidth() { return m_width; }
			const int GetHeight() { return m_height; }

		protected:
			GLuint m_buffer;
			bool m_isBound;
			int m_width, m_height;

			std::vector<GLenum> m_attachments;
		};
	}
}


#endif