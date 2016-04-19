#ifndef _FRAMEBUFFER_H_
#define _FRAMEBUFFER_H_

#include "../../Common.h"

namespace tactical
{
	namespace render
	{
		class FramebufferTexture
		{
		public:
			FramebufferTexture();
			FramebufferTexture(GLenum internalFormat, int width, int height, GLenum format, GLenum type);
			~FramebufferTexture();

			void Bind();
			void Unbind();

			void SetTexture(GLenum internalFormat, int width, int height, GLenum format, GLenum type);

			GLuint GetId() { return m_texture; }

		private:
			GLuint m_texture;
		};

		class RenderBuffer
		{
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
		};

		class Framebuffer
		{
		public:
			Framebuffer();
			~Framebuffer();

			void Bind(GLenum target = GL_FRAMEBUFFER);
			void Unbind(GLenum target = GL_FRAMEBUFFER);

			void AttachColourbuffer(FramebufferTexture& texture, GLenum colourAttachment);
			void AttachRenderBuffer(RenderBuffer& buffer, GLenum renderAttachment);

			void Blit(int width, int height, GLenum bufferBit = GL_DEPTH_BUFFER_BIT, GLenum filter = GL_NEAREST);

			bool CheckStatus();

		protected:
			GLuint m_buffer;
			bool m_isBound;

			std::vector<GLenum> m_attachments;
		};
	}
}


#endif