#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "../Common.h"
#include "../utils/Utils.h"

#include "camera\Camera.h"

#include "buffer\Buffer.h"
#include "buffer\IndexBuffer.h"
#include "buffer\VertexArray.h"
#include "buffer\VertexAttribute.h"

#include "IRenderable2D.h"
#include "IRenderable3D.h"

#include "structures\Shader.h"
#include "structures\Vertex.h"
#include "structures\Mesh.h"

#include "geometry\Geometry.h"

namespace tactical
{
	namespace render
	{
		enum PolygonMode
		{
			POLYGON = 0,
			POLYGON_AND_WIREFRAME,
			WIREFRAME
		};

		class Renderer
		{
		public:
			Renderer(Camera* camera, std::vector<std::string> shaderPaths);
			~Renderer();

			void Render(IRenderable3D& renderable, std::string shaderID);

			void SetPolygonMode();
			void SetNormalRendering() { m_showNormals = !m_showNormals; }

		private:
			Renderer();

		private:
			Camera* m_pCamera;
			std::unordered_map<std::string, Shader*> m_shaders;

			PolygonMode m_polygonMode;
			bool m_showNormals;
		};
	}
}

#endif