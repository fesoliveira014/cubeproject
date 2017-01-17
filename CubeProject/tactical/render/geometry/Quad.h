#ifndef _QUAD_H_
#define _QUAD_H_

#include "../../Common.h"

#include "Geometry.h"

#include "../IRenderable2D.h"
#include "../structures/Shader.h"
#include "../structures/Mesh.h"
#include "../structures/Vertex.h"

#include "../buffer/Framebuffer.h" 

namespace tactical
{
	namespace render
	{
		template <typename TextureType>
		class Quad : public IRenderable2D
		{
		public:
			Quad();
			~Quad();

			void Draw(Shader& shader);

			TextureType * GetTexture() { return m_texture; }
			void SetTexture(TextureType* texture) { m_texture = texture; LOG_INFO("TextureID: " + std::to_string(m_texture->GetId())); }

		protected:
			Mesh<Vertex3f2f> m_mesh;
			TextureType* m_texture;
		};
	}
}

#include  "Quad.impl"

#endif
