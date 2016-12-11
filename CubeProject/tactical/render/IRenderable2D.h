#ifndef _IRENDERABLE2D_H_
#define _IRENDERABLE2D_H_

#include "../Common.h"
#include "../utils/Utils.h"

#include "buffer/Buffer.h"
#include "buffer/IndexBuffer.h"
#include "buffer/VertexArray.h"

#include "structures\Shader.h"

namespace tactical
{
	namespace render
	{
		class IRenderable2D
		{
		public:
			IRenderable2D() {}
			IRenderable2D(glm::vec3 position, glm::vec2 size, glm::vec4 color);
			virtual ~IRenderable2D();

			virtual void Draw(Shader& shader) = 0;

			inline const glm::vec3 GetPosition() const { return m_position; }
			inline const glm::vec2 GetSize() const { return m_size; }
			inline const glm::vec4 GetColor() const { return m_color; }

		protected:
			glm::vec3 m_position;  // relative to top left corner
			glm::vec2 m_size;	   // dimension
			glm::vec4 m_color;     // color

		};
	}
}

#endif