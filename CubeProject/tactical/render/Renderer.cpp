#include  "Renderer.h"

namespace tactical
{
	namespace render
	{
		Renderer::Renderer(Camera* camera, std::vector<std::string> shaderPaths)
		{
			m_pCamera = camera;

			m_shaders["basic_light"] = new Shader("shaders/vertex.glsl", "shaders/frag.glsl", nullptr);
			m_shaders["normal"] = new Shader("shaders/normal_vertex.glsl", "shaders/normal_frag.glsl", "shaders/normal_geom.glsl");
			m_polygonMode = POLYGON;
		}

		void Renderer::Render(IRenderable3D& renderable, std::string shaderID)
		{
			if (!(m_shaders.find(shaderID) == m_shaders.end()))
				renderable.Draw(*m_shaders[shaderID]);
			else
				renderable.Draw(*m_shaders["basic_light"]);
		}

		void Renderer::SetPolygonMode()
		{
			m_polygonMode = PolygonMode((m_polygonMode + 1) % 3);
		}
	}
}