#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "../Common.h"
#include "../utils/Utils.h"

#include "camera\Camera.h"

#include "buffer\Buffer.h"
#include "buffer\IndexBuffer.h"
#include "buffer\VertexArray.h"
#include "buffer\VertexAttribute.h"
#include "buffer\Framebuffer.h"

#include "IRenderable2D.h"
#include "IRenderable3D.h"

#include "structures\Shader.h"
#include "structures\Vertex.h"
#include "structures\Mesh.h"
#include "structures\Light.h"

#include "../math/AABB.h"
#include "../math/Frustum.h"
#include "../math/Ray.h"

#include "geometry\Geometry.h"
#include "geometry\Quad.h"


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

			void PreRender();
			void Render(std::shared_ptr<IRenderable3D>& renderable, std::string shaderID);
			void PostRender();

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

			void ToggleFog();

			void SelectLightType(int type);
			int GetLightType() { return m_lightType; }

			void SetPointLightPosition(const glm::vec3& position);
			void SetSpotLightPosition(const glm::vec3& position);
			void SetSpotLightDirection(const glm::vec3& direction);
			void SetSpotLightPositionAndDirection(const glm::vec3& position, const glm::vec3& direction);

			void SetActiveTexture(GLTexture slot);

			void Update(float deltaTime);

			void EnableShader(std::string shaderID) { m_shaders[shaderID]->Enable(); }
			void DisableShader(std::string shaderID) { m_shaders[shaderID]->Disable(); }

			Shader* GetShader(std::string shaderID);
			FramebufferTexture* GetFramebufferTexture(std::string fbID);

			void LinkTo(window::Window& windowHandler);

			void SetCamera(Camera* camera) { m_pCamera = camera; }

		private:
			Renderer();
			
			void SetupFramebuffers();

		private:
			Camera* m_pCamera;
			std::shared_ptr<window::EventHandler> m_eventHandler;
			std::unordered_map<std::string, Shader*> m_shaders;
			std::unordered_map<std::string, Framebuffer*> m_framebuffers;
			std::unordered_map<std::string, FramebufferTexture*> m_framebufferTextures;
			std::unordered_map<std::string, RenderBuffer*> m_renderBuffers;

			std::shared_ptr<FramebufferTexture> m_depthMap;
			std::shared_ptr<RenderBuffer> m_depthRenderBuffer;
			Quad<FramebufferTexture> m_quad;


			int m_lightType;

			DirectionalLight m_directionalLight;
			PointLight m_pointLight;
			SpotLight m_spotLight;

			PolygonMode m_polygonMode;
			bool m_showNormals;
			bool m_renderFog;
			math::Frustum m_frustum;
		};
	}
}

#endif