#include "IRenderable2D.h"

namespace tactical
{
	namespace render
	{
		IRenderable2D::IRenderable2D(glm::vec3 position, glm::vec2 size, glm::vec4 color)
			: m_position(position), m_size(size), m_color(color)
		{

		}

		IRenderable2D::~IRenderable2D()
		{

		}
	}
}