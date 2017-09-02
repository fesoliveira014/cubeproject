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
			m_shaders["depthMap"] = new Shader("shaders/depth.vert.glsl", "shaders/depth.frag.glsl", nullptr);
			m_shaders["depthDebug"] = new Shader("shaders/depthDebugQuad.vert.glsl", "shaders/depthDebugQuad.frag.glsl", nullptr);
			m_polygonMode = POLYGON;
			m_renderFog = false;
			m_lightType = 0;

			m_directionalLight.position = glm::vec3(-16.0f, 64.0f, -16.0f);
			m_directionalLight.direction = glm::normalize(glm::vec3(0.0f, 0.0f, 0.0f) - m_directionalLight.position);

			m_shaders["basic_light"]->Enable();
			m_shaders["basic_light"]->SetUniformMat4fv("projection", m_pCamera->GetProjectionMatrix());
			m_shaders["basic_light"]->SetUniformMat4fv("model", glm::mat4(1.0f));
			m_shaders["basic_light"]->SetUniform3fv("camera_pos", m_pCamera->GetPosition());
			m_shaders["basic_light"]->SetUniformBool("fog_enabled", m_renderFog);
			m_shaders["basic_light"]->SetUniform1f("gamma", 2.2f);
			m_shaders["basic_light"]->SetUniform1i("light_type", m_lightType);

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

			m_shaders["basic_light"]->SetUniform1i("shadowMap", 0);

			m_shaders["normal"]->Enable();
			m_shaders["normal"]->SetUniformMat4fv("projection", m_pCamera->GetProjectionMatrix());
			m_shaders["normal"]->SetUniformMat4fv("model", glm::mat4(1.0f));

			m_shaders["picking"]->Enable();
			m_shaders["picking"]->SetUniformMat4fv("projection", m_pCamera->GetProjectionMatrix());
			m_shaders["picking"]->SetUniformMat4fv("model", glm::mat4(1.0f));

			m_shaders["depthMap"]->Enable();
			m_shaders["depthMap"]->SetUniformMat4fv("model", glm::mat4(1.0f));
			//m_shaders["depthMap"]->SetUniform1i("depthMap", (GLint)0);

			m_shaders["depthDebug"]->Enable();
			m_shaders["depthDebug"]->SetUniform1i("depthMap", (GLint)1);
			//m_shaders["depthDebug"]->SetUniform1f("near_plane", (GLfloat)1.0f);
			//m_shaders["depthDebug"]->SetUniform1f("far_plane", (GLfloat)1.0f);
			
			m_renderPass = RenderPass::TERRAIN;
			//SetupFramebuffers();

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

			std::unordered_map<std::string, Framebuffer*>::iterator fbit;
			for (fbit = m_framebuffers.begin(); fbit != m_framebuffers.end(); ++fbit) {
				delete (*fbit).second;
			}

			std::unordered_map<std::string, FramebufferTexture*>::iterator fbtit;
			for (fbtit = m_framebufferTextures.begin(); fbtit != m_framebufferTextures.end(); ++fbtit) {
				delete (*fbtit).second;
			}

			std::unordered_map<std::string, RenderBuffer*>::iterator rbit;
			for (rbit = m_renderBuffers.begin(); rbit != m_renderBuffers.end(); ++rbit) {
				delete (*rbit).second;
			}

			m_pCamera = nullptr;
		}
		
		void Renderer::SetupFramebuffers()
		{
			if (m_framebuffers["depthMap"] != nullptr) delete m_framebuffers["depthMap"];
			if (m_framebufferTextures["depthMap"] != nullptr) delete m_framebufferTextures["depthMap"];

			int height = 8192;//m_eventHandler->GetWindowSizeState()->height;
			int width = 8192;// m_eventHandler->GetWindowSizeState()->width;
			glm::vec4 borderColor = glm::vec4(1.0f);

			m_framebuffers["depthMap"] = new Framebuffer();
			m_framebufferTextures["depthMap"] = new FramebufferTexture(GL_DEPTH_COMPONENT, width, height, GL_DEPTH_COMPONENT, GL_FLOAT);

			m_framebufferTextures["depthMap"]->SetParameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			m_framebufferTextures["depthMap"]->SetParameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			m_framebufferTextures["depthMap"]->SetParameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			m_framebufferTextures["depthMap"]->SetParameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			m_framebufferTextures["depthMap"]->SetParameterfv(GL_TEXTURE_BORDER_COLOR, glm::value_ptr(borderColor));
			
			m_framebuffers["depthMap"]->AttachColourbuffer(*m_framebufferTextures["depthMap"], GL_DEPTH_ATTACHMENT);

			m_framebuffers["depthMap"]->CheckStatus();
		}

		void Renderer::Render(std::shared_ptr<IRenderable3D>& renderable, std::string shaderID)
		{
			if (m_polygonMode == WIREFRAME)
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			else
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			if (renderable->GetBoundingBox().Contains(m_pCamera->GetPosition()) || (m_renderPass == RenderPass::SHADOW || m_frustum.Contains(renderable->GetBoundingBox()))) {
				if (!(m_shaders.find(shaderID) == m_shaders.end()))
					renderable->Draw(*m_shaders[shaderID]);
				else
					renderable->Draw(*m_shaders["basic_light"]);
			}
		}

		// TODO: finish
		void Renderer::PreRender()
		{
			glm::mat4 invCamera = glm::inverse(m_pCamera->GetViewMatrix());

			float lightPosDegree = 170.0f * glm::pi<float>() / 180.0f;

			m_directionalLight.position = m_pCamera->GetPosition() + glm::vec3(
				glm::cos(lightPosDegree) * 1000.0f, 
				glm::sin(lightPosDegree) * 1000.0f,
				glm::sin(lightPosDegree + 90) * 1000.0f / 2.0f);

			glm::mat4 lightView = glm::lookAt(
				m_pCamera->GetPosition() + 
				glm::normalize(m_directionalLight.position - m_pCamera->GetPosition()) * (1024.0f / 2.0f),
											  m_pCamera->GetPosition(),
											  glm::vec3(0.0f, 1.0f, 0.0f));

			lightView = glm::rotate(glm::mat4(1.0f), glm::radians(m_pCamera->GetYaw()), glm::vec3(0.0f, 0.0f, 1.0f)) * lightView;
			
			float dimOrtho = 128.0f;
			glm::mat4 lightProjection = glm::ortho(-dimOrtho, dimOrtho, -dimOrtho, dimOrtho, 1.0f, 1024.0f);
			glm::mat4 lightSpaceMatrix = lightProjection * lightView;
			
			m_shaders["basic_light"]->Enable();
			m_shaders["basic_light"]->SetUniformMat4fv("lightViewProjection", lightSpaceMatrix);
			m_shaders["basic_light"]->SetUniform3fv("dirLight.position", m_directionalLight.position);
			m_shaders["basic_light"]->Disable();

			m_framebuffers["depthMap"]->Bind();

			m_shaders["depthMap"]->Enable();
			m_shaders["depthMap"]->SetUniformMat4fv("lightViewProjection", lightSpaceMatrix);

			m_renderPass = RenderPass::SHADOW;

			glViewport(0, 0, m_framebufferTextures["depthMap"]->GetWidth(), m_framebufferTextures["depthMap"]->GetHeight());
			SetActiveTexture(tactical::GLTexture::TEXTURE0);
			m_framebuffers["depthMap"]->Bind();
			glClear(GL_DEPTH_BUFFER_BIT);
			glCullFace(GL_FRONT);
			glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		}

		// TODO: start
		void Renderer::PostRender()
		{
			m_framebuffers["depthMap"]->Unbind();
			m_shaders["depthMap"]->Disable();

			glClearColor(135.0f / 255.0f, 206.0f / 255.0f, 250.0f / 255.0f, 1.0f);
			glCullFace(GL_BACK);
			glViewport(0, 0, m_eventHandler->GetWindowSizeState()->width, m_eventHandler->GetWindowSizeState()->height);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			m_renderPass = RenderPass::TERRAIN;

		}

		void Renderer::TogglePolygonMode()
		{
			if (m_polygonMode == POLYGON)
				m_polygonMode = WIREFRAME;
			else
				m_polygonMode = POLYGON;
		}

		void Renderer::Update(float deltaTime)
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
			SetupFramebuffers();
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

			if (!m_shaders["basic_light"]->IsEnabled()) m_shaders["basic_light"]->Enable();
			m_shaders["basic_light"]->SetUniform1i("light_type", m_lightType);
		}

		void Renderer::SetPointLightPosition(const glm::vec3& position) 
		{
			m_pointLight.position = position;

			if (!m_shaders["basic_light"]->IsEnabled()) m_shaders["basic_light"]->Enable();
			m_shaders["basic_light"]->SetUniform3fv("pointLight.position", m_pointLight.position);
		}

		void Renderer::SetSpotLightPosition(const glm::vec3& position)
		{
			m_spotLight.position = position;

			if (!m_shaders["basic_light"]->IsEnabled()) m_shaders["basic_light"]->Enable();
			m_shaders["basic_light"]->SetUniform3fv("spotLight.position", m_spotLight.position);
		}

		void Renderer::SetSpotLightDirection(const glm::vec3& direction)
		{
			m_spotLight.direction = direction;

			if (!m_shaders["basic_light"]->IsEnabled()) m_shaders["basic_light"]->Enable();
			m_shaders["basic_light"]->SetUniform3fv("spotLight.direction", m_spotLight.direction);
		}

		void Renderer::SetSpotLightPositionAndDirection(const glm::vec3& position, const glm::vec3& direction)
		{
			SetSpotLightPosition(position);
			SetSpotLightDirection(direction);
		}

		void Renderer::SetActiveTexture(GLTexture slot)
		{
			glActiveTexture((GLenum)slot);
		}

		void Renderer::BindTexture(std::string handle)
		{
			if (m_framebufferTextures.find(handle) != m_framebufferTextures.end()) {
				m_framebufferTextures[handle]->Bind();
			}
		}

		void Renderer::ReleaseTexture(std::string handle)
		{
			if (m_framebufferTextures.find(handle) != m_framebufferTextures.end()) {
				m_framebufferTextures[handle]->Unbind();
			}
		}

		void Renderer::ToggleFog()
		{
			m_renderFog = !m_renderFog;

			if (!m_shaders["basic_light"]->IsEnabled()) m_shaders["basic_light"]->Enable();
			m_shaders["basic_light"]->SetUniformBool("fog_enabled", m_renderFog);
		}

		Shader* Renderer::GetShader(std::string shaderID) 
		{ 
			if (!(m_shaders.find(shaderID) == m_shaders.end()))
				return m_shaders[shaderID];
			else
				return nullptr; // TODO: add error catching
		}
		
		FramebufferTexture* Renderer::GetFramebufferTexture(std::string fbID) 
		{ 
			if (!(m_framebufferTextures.find(fbID) == m_framebufferTextures.end()))
				return m_framebufferTextures[fbID];
			else
				return nullptr; // TODO: add error catching
		}

		
	}
}