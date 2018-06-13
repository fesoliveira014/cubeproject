#include "Filter.h"

namespace tactical
{
	namespace render
	{
		Filter::Filter(std::string name, int width, int height)
		{
			m_framebuffer = Framebuffer();

			m_output = FramebufferTexture(GL_RG32F, width, height, GL_RG, GL_FLOAT);
			m_output.SetParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			m_output.SetParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			m_framebuffer.AttachColourbuffer(m_output, GL_COLOR_ATTACHMENT0);
			m_framebuffer.CheckStatus();

			m_name = name;

			m_canvas.SetTexture(&m_output);
		}

		void Filter::ApplyFilter(FramebufferTexture& texture, Shader& shader)
		{
			glViewport(0, 0, m_width, m_height);

			m_framebuffer.Bind();
			texture.Bind();

			//shader.SetUniform2fv("viewport", glm::vec2(m_width, m_height));
			shader.SetUniform1i("textureSource", texture.GetId());
			m_canvas.Draw(shader);

			m_framebuffer.Unbind();
		}

	}
}