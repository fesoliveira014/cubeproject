#include  "Renderer.h"

namespace tactical
{
	namespace render
	{
		Renderer::Renderer(Camera* camera)
		{
			m_pCamera = camera;

			m_shaders["basic_light"] = new Shader("shaders/vertex.glsl", "shaders/frag.glsl", nullptr);
			m_shaders["normal"] = new Shader("shaders/normal_vertex.glsl", "shaders/normal_frag.glsl", "shaders/normal_geom.glsl");
			m_polygonMode = POLYGON;

			m_shaders["basic_light"]->Enable();
			m_shaders["basic_light"]->SetUniformMat4fv("projection", m_pCamera->GetProjectionMatrix());
			m_shaders["basic_light"]->SetUniformMat4fv("model", glm::mat4(1.0f));
			m_shaders["basic_light"]->SetUniform3fv("light_pos", glm::vec3(-6.0f, 70.0f, 21.0f));
			m_shaders["basic_light"]->SetUniform3fv("light_color", glm::vec3(1.0f, 1.0f, 1.0f));
			m_shaders["basic_light"]->SetUniform3fv("camera_pos", camera->GetPosition());

			m_shaders["normal"]->Enable();
			m_shaders["normal"]->SetUniformMat4fv("projection", m_pCamera->GetProjectionMatrix());
			m_shaders["normal"]->SetUniformMat4fv("model", glm::mat4(1.0f));

			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			glFrontFace(GL_CCW);

			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LESS);
		}

		Renderer::~Renderer()
		{
			std::unordered_map<std::string, Shader*>::iterator it;
			for (it = m_shaders.begin(); it != m_shaders.end(); ++it) {
				delete (*it).second;
			}

			m_pCamera = nullptr;
		}

		void Renderer::Render(IRenderable3D* renderable, std::string shaderID)
		{
			if (m_polygonMode == WIREFRAME)
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINES);

			if (renderable->GetBoundingBox().Contains(m_pCamera->GetPosition()) || m_frustum.Contains(renderable->GetBoundingBox())) {
				if (!(m_shaders.find(shaderID) == m_shaders.end()))
					renderable->Draw(*m_shaders[shaderID]);
				else
					renderable->Draw(*m_shaders["basic_light"]);
			}
		}

		void Renderer::TogglePolygonMode()
		{
			m_polygonMode = PolygonMode((m_polygonMode + 1) % 3);
		}

		void Renderer::Update()
		{
			m_frustum.Update(m_pCamera->GetProjectionMatrix(), m_pCamera->GetViewMatrix());
		}
	}
}