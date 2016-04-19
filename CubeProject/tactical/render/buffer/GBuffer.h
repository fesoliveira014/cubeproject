#ifndef _G_BUFFER_H_
#define _G_BUFFER_H_

#include "../../Common.h"
#include "Framebuffer.h"

namespace tactical
{
	namespace render
	{
		class GBuffer : public Framebuffer
		{
		public:
			GBuffer(int width, int height);
			~GBuffer();

		private:
			FramebufferTexture m_gPosition, m_gNormal, m_gColor;
			RenderBuffer m_gRender;
		};
	}
}

#endif