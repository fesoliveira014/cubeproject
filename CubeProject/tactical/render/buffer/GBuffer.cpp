#include "GBuffer.h"

namespace tactical
{
	namespace render
	{
		GBuffer::GBuffer(int width, int height)
		{
			Framebuffer();
			
			m_gPosition.SetTexture(GL_RGB16F, width, height, GL_RGB, GL_FLOAT);
			m_gNormal.SetTexture(GL_RGB16F, width, height, GL_RGB, GL_FLOAT);
			m_gColor.SetTexture(GL_RGBA, width, height, GL_RGBA, GL_FLOAT);
			m_gRender.SetRenderBuffer(GL_DEPTH_COMPONENT, width, height);

			Bind();

			AttachColourbuffer(m_gPosition, GL_COLOR_ATTACHMENT0);
			AttachColourbuffer(m_gNormal, GL_COLOR_ATTACHMENT1);
			AttachColourbuffer(m_gColor, GL_COLOR_ATTACHMENT2);

			glDrawBuffers(m_attachments.size(), m_attachments.data());

			AttachRenderBuffer(m_gRender, GL_DEPTH_ATTACHMENT);

			CheckStatus();

			Unbind();
		}

		GBuffer::~GBuffer()
		{

		}
	}
}