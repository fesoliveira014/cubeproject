#ifndef _FILTER_H_
#define _FILTER_H_

#include "../../Common.h"

#include "../buffer/Framebuffer.h"
#include "../structures/Shader.h"
#include "../geometry/Quad.h"

namespace tactical
{
	namespace render
	{
		class Filter
		{
		public:
			Filter(std::string name, int width, int height);
			~Filter() {};

			void ApplyFilter(FramebufferTexture& texture, Shader& shader);

			FramebufferTexture GetTexture() { return m_output; }

		private:
			Framebuffer m_framebuffer;
			FramebufferTexture m_output;
			int m_width, m_height;
			std::string m_name;

			Quad<FramebufferTexture> m_canvas;
		};
	}
}

#endif