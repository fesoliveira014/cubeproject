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

#include "../math/AABB.h"
#include "../math/Frustum.h"
#include "../math/Ray.h"

#include "geometry\Geometry.h"


namespace tactical
{
	namespace render
	{
		enum PolygonMode
		{
			WIREFRAME = 0,
			POLYGON_AND_WIREFRAME,
			POLYGON
		};

		class Renderer
		{
		public:
			Renderer(Camera* camera);
			~Renderer();

			void Render(std::shared_ptr<IRenderable3D>& renderable, std::string shaderID);

			void TogglePolygonMode();
			PolygonMode GetPolygonMode() { return m_polygonMode; }

			void WireframeMode()
			{
				if (m_polygonMode == WIREFRAME)
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				else 
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}

			void FillPolygonMode()
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}

			void ToggleNormalRendering() { m_showNormals = !m_showNormals; }
			bool NormalRendering() { return m_showNormals; }

			void Update();

			void EnableShader(std::string shaderID) { m_shaders[shaderID]->Enable(); }
			void DisableShader(std::string shaderID) { m_shaders[shaderID]->Disable(); }

			Shader* GetShader(std::string shaderID) { return m_shaders[shaderID]; }

		private:
			Renderer();

		private:
			Camera* m_pCamera;
			std::unordered_map<std::string, Shader*> m_shaders;

			PolygonMode m_polygonMode;
			bool m_showNormals;
			math::Frustum m_frustum;
		};
	}
}

#endif