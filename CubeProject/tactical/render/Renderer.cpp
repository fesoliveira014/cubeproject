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
			m_shaders["picking"] = new Shader("shaders/picking_vert.glsl", "shaders/picking_frag.glsl", nullptr);
			m_polygonMode = POLYGON;
			m_renderFog = false;

			m_shaders["basic_light"]->Enable();
			m_shaders["basic_light"]->SetUniformMat4fv("projection", m_pCamera->GetProjectionMatrix());
			m_shaders["basic_light"]->SetUniformMat4fv("model", glm::mat4(1.0f));
			m_shaders["basic_light"]->SetUniform3fv("camera_pos", m_pCamera->GetPosition());
			m_shaders["basic_light"]->SetUniformBool("fog_enabled", m_renderFog);
			m_shaders["basic_light"]->SetUniform1f("gamma", 2.2f);

			m_shaders["basic_light"]->SetUniform3fv("dirLight.position", m_directionalLight.position);
			m_shaders["basic_light"]->SetUniform4fv("dirLight.color", m_directionalLight.color);
			m_shaders["basic_light"]->SetUniform3fv("dirLight.direction", m_directionalLight.direction);
			m_shaders["basic_light"]->SetUniform3fv("dirLight.ambient", m_directionalLight.ambient);
			m_shaders["basic_light"]->SetUniform3fv("dirLight.diffuse", m_directionalLight.diffuse);
			m_shaders["basic_light"]->SetUniform3fv("dirLight.specular", m_directionalLight.specular);

			m_shaders["basic_light"]->SetUniform3fv("pointLight.position",  m_pointLight.position);
			m_shaders["basic_light"]->SetUniform4fv("pointLight.color",     m_pointLight.color);
			m_shaders["basic_light"]->SetUniform3fv("pointLight.ambient",   m_pointLight.ambient);
			m_shaders["basic_light"]->SetUniform3fv("pointLight.diffuse",   m_pointLight.diffuse);
			m_shaders["basic_light"]->SetUniform3fv("pointLight.specular",  m_pointLight.specular);
			m_shaders["basic_light"]->SetUniform1f("pointLight.constant",   m_pointLight.constant);
			m_shaders["basic_light"]->SetUniform1f("pointLight.linear",     m_pointLight.linear);
			m_shaders["basic_light"]->SetUniform1f("pointLight.quadratic",  m_pointLight.quadratic);

			m_shaders["basic_light"]->SetUniform3fv("spotLight.position",    m_spotLight.position);
			m_shaders["basic_light"]->SetUniform4fv("spotLight.color",       m_spotLight.color);
			m_shaders["basic_light"]->SetUniform3fv("spotLight.direction",   m_spotLight.direction);
			m_shaders["basic_light"]->SetUniform3fv("spotLight.ambient",     m_spotLight.ambient);
			m_shaders["basic_light"]->SetUniform3fv("spotLight.diffuse",     m_spotLight.diffuse);
			m_shaders["basic_light"]->SetUniform3fv("spotLight.specular",    m_spotLight.specular);
			m_shaders["basic_light"]->SetUniform1f("spotLight.constant",     m_spotLight.constant);
			m_shaders["basic_light"]->SetUniform1f("spotLight.linear",       m_spotLight.linear);
			m_shaders["basic_light"]->SetUniform1f("spotLight.quadratic",	 m_spotLight.quadratic);
			m_shaders["basic_light"]->SetUniform1f("spotLight.cutOff",       m_spotLight.cutOff);
			m_shaders["basic_light"]->SetUniform1f("spotLight.outerCutOff",  m_spotLight.outerCutOff);

			m_shaders["basic_light"]->SetUniform3fv("dirLight.position", m_directionalLight.position);
			m_shaders["basic_light"]->SetUniform4fv("dirLight.color", m_directionalLight.color);
			m_shaders["basic_light"]->SetUniform3fv("dirLight.direction", m_directionalLight.direction);
			m_shaders["basic_light"]->SetUniform3fv("dirLight.ambient", m_directionalLight.ambient);
			m_shaders["basic_light"]->SetUniform3fv("dirLight.diffuse", m_directionalLight.diffuse);
			m_shaders["basic_light"]->SetUniform3fv("dirLight.specular", m_directionalLight.specular);

			m_shaders["normal"]->Enable();
			m_shaders["normal"]->SetUniformMat4fv("projection", m_pCamera->GetProjectionMatrix());
			m_shaders["normal"]->SetUniformMat4fv("model", glm::mat4(1.0f));

			m_shaders["picking"]->Enable();
			m_shaders["picking"]->SetUniformMat4fv("projection", m_pCamera->GetProjectionMatrix());
			m_shaders["picking"]->SetUniformMat4fv("model", glm::mat4(1.0f));

			m_lightType = 0;

			m_framebuffers["depthMap"] = new Framebuffer();

			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			glFrontFace(GL_CCW);

			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LESS);

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glClearColor(135.0f / 255.0f, 206.0f / 255.0f, 250.0f / 255.0f, 1.0f);
		}

		Renderer::~Renderer()
		{
			std::unordered_map<std::string, Shader*>::iterator it;
			for (it = m_shaders.begin(); it != m_shaders.end(); ++it) {
				delete (*it).second;
			}

			m_pCamera = nullptr;
		}
		
		void Renderer::SetupFramebuffers()
		{
			int height = m_eventHandler->GetWindowSizeState()->height;
			int width = m_eventHandler->GetWindowSizeState()->width;

			FramebufferTexture depthMap(GL_TEXTURE_2D, width, height, GL_DEPTH_COMPONENT, GL_FLOAT);
			depthMap.SetParameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			depthMap.SetParameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			depthMap.SetParameteri(GL_TEXTURE_WRAP_S, GL_REPEAT);
			depthMap.SetParameteri(GL_TEXTURE_WRAP_T, GL_REPEAT);

			m_framebuffers["depthMap"]->AttachColourbuffer(depthMap, GL_DEPTH_ATTACHMENT);
			m_framebuffers["depthMap"]->CheckStatus();
		}

		void Renderer::Render(std::shared_ptr<IRenderable3D>& renderable, std::string shaderID)
		{
			if (m_polygonMode == WIREFRAME)
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			else
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			if (renderable->GetBoundingBox().Contains(m_pCamera->GetPosition()) || m_frustum.Contains(renderable->GetBoundingBox())) {
				if (!(m_shaders.find(shaderID) == m_shaders.end()))
					renderable->Draw(*m_shaders[shaderID]);
				else
					renderable->Draw(*m_shaders["basic_light"]);
			}
		}

		void Renderer::RenderToQuad(std::shared_ptr<IRenderable3D>& renderable, std::string shaderID)
		{

		}

		void Renderer::TogglePolygonMode()
		{
			if (m_polygonMode == POLYGON)
				m_polygonMode = WIREFRAME;
			else
				m_polygonMode = POLYGON;
		}

		void Renderer::Update()
		{
			GLenum errorGL = glGetError();
			if (errorGL != GL_NO_ERROR) {
				while (errorGL != GL_NO_ERROR) {
					LOG << LOGTYPE::LOG_WARNING << "OpenGL error: " + std::to_string(errorGL);
					errorGL = glGetError();
				}
			}

			m_frustum.Update(m_pCamera->GetProjectionMatrix(), m_pCamera->GetViewMatrix());
		}

		void Renderer::LinkTo(window::Window& windowHandler)
		{
			m_eventHandler = windowHandler.GetEventHandler();
		}

		

		void Renderer::SelectLightType(int type)
		{
			switch (type) {
			default:
			case 0:
				m_lightType = 0;
				break;
			case 1:
				m_lightType = 1;
				break;
			case 2:
				m_lightType = 2;
				break;
			}

			m_shaders["basic_light"]->Enable();
			m_shaders["basic_light"]->SetUniform1i("light_type", m_lightType);
		}

		void Renderer::SetPointLightPosition(const glm::vec3& position) 
		{
			m_pointLight.position = position;

			m_shaders["basic_light"]->Enable();
			m_shaders["basic_light"]->SetUniform3fv("pointLight.position", m_pointLight.position);
		}

		void Renderer::SetSpotLightPosition(const glm::vec3& position)
		{
			m_spotLight.position = position;

			m_shaders["basic_light"]->Enable();
			m_shaders["basic_light"]->SetUniform3fv("spotLight.position", m_spotLight.position);
		}

		void Renderer::SetSpotLightDirection(const glm::vec3& direction)
		{
			m_spotLight.direction = direction;

			m_shaders["basic_light"]->Enable();
			m_shaders["basic_light"]->SetUniform3fv("spotLight.direction", m_spotLight.direction);
		}

		void Renderer::SetSpotLightPositionAndDirection(const glm::vec3& position, const glm::vec3& direction)
		{
			SetSpotLightPosition(position);
			SetSpotLightDirection(direction);
		}

		void Renderer::ToggleFog()
		{
			m_renderFog = !m_renderFog;

			m_shaders["basic_light"]->Enable();
			m_shaders["basic_light"]->SetUniformBool("fog_enabled", m_renderFog);
		}


	}
}