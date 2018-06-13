#include  "Renderer.h"

namespace tactical
{
	namespace render
	{
		Renderer::Renderer(Camera* camera)
		{
			m_pCamera = camera;

			m_polygonMode = POLYGON;
			m_renderFog = false;
			m_lightType = 0;

			m_directionalLight.position = glm::vec3(-16.0f, 32.0f, -16.0f);
			m_directionalLight.direction = glm::normalize(-m_directionalLight.position);

			ReloadShaders();

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
			for (auto it = m_shaders.begin(); it != m_shaders.end(); ++it) {
				delete (*it).second;
			}

			for (auto fbit = m_framebuffers.begin(); fbit != m_framebuffers.end(); ++fbit) {
				delete (*fbit).second;
			}

			for (auto fbtit = m_framebufferTextures.begin(); fbtit != m_framebufferTextures.end(); ++fbtit) {
				delete (*fbtit).second;
			}

			for (auto rbit = m_renderBuffers.begin(); rbit != m_renderBuffers.end(); ++rbit) {
				delete (*rbit).second;
			}

			m_pCamera = nullptr;
		}
		
		void Renderer::SetupFramebuffers()
		{
			if (m_framebuffers["depthMap"] != nullptr) delete m_framebuffers["depthMap"];
			if (m_framebufferTextures["depthMap"] != nullptr) delete m_framebufferTextures["depthMap"];

			int height = 8192;
			int width = 8192;
			glm::vec4 borderColor = glm::vec4(1.0f);

			m_framebuffers["depthMap"] = new Framebuffer();

			m_framebufferTextures["depthMap"] = new FramebufferTexture(GL_DEPTH_COMPONENT, width, height, GL_DEPTH_COMPONENT, GL_FLOAT);
			m_framebufferTextures["depthMap"]->SetParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			m_framebufferTextures["depthMap"]->SetParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
				if (!(m_shaders.find(shaderID) == m_shaders.end())) {
					EnableShader(shaderID);
					renderable->Draw(*m_shaders[shaderID]);
				}
				else {
					LOG_WARNING("Shader " + shaderID + " not found.");
					EnableShader("basic_light");
					renderable->Draw(*m_shaders["basic_light"]);
				}
			}
		}

		// TODO: finish
		void Renderer::ShadowPassStart()
		{
			//glm::mat4 invCamera = glm::inverse(m_pCamera->GetViewMatrix());

			std::chrono::system_clock::time_point tp = std::chrono::system_clock::now();
			std::chrono::system_clock::duration duration = tp.time_since_epoch();

			float lightPosDegree = (170.0f * glm::pi<float>() / 180.0f) * duration.count() * std::chrono::system_clock::period::num / std::chrono::system_clock::period::den;

			m_directionalLight.position = m_pCamera->GetPosition() + glm::vec3(
				glm::cos(lightPosDegree), 
				glm::sin(lightPosDegree),
				glm::sin(lightPosDegree + 90) / 2.0f);

			glm::vec3 position = m_pCamera->GetPosition(); //m_directionalLight.position;
			glm::vec3 light_dir = m_directionalLight.direction;
			
			glm::vec3 target = position + light_dir;

			glm::mat4 lightView = glm::lookAt(//position, target, 
				m_pCamera->GetPosition() + 
				glm::normalize(m_directionalLight.position - m_pCamera->GetPosition()) * (1024.0f / 2.0f),
											  m_pCamera->GetPosition(),
											  glm::vec3(0.0f, 1.0f, 0.0f));

			lightView = glm::rotate(glm::mat4(1.0f), glm::radians(m_pCamera->GetYaw()), glm::vec3(0.0f, 0.0f, 1.0f)) * lightView;
			
			float dimOrtho = 256.0f;
			glm::mat4 lightProjection = glm::ortho(-dimOrtho, dimOrtho, -dimOrtho, dimOrtho, 1.0f, 1024.0f);
			glm::mat4 lightSpaceMatrix = lightProjection * lightView;
			
			EnableShader("basic_light");
			m_shaders["basic_light"]->SetUniformMat4fv("lightViewProjection", lightSpaceMatrix);
			m_shaders["basic_light"]->SetUniform3fv("dirLight.position", m_directionalLight.position);
			m_shaders["basic_light"]->Disable();

			m_framebuffers["depthMap"]->Bind();

			EnableShader("depthMap");
			m_shaders["depthMap"]->SetUniformMat4fv("lightViewProjection", lightSpaceMatrix);

			m_renderPass = RenderPass::SHADOW;

			glViewport(0, 0, m_framebufferTextures["depthMap"]->GetWidth(), m_framebufferTextures["depthMap"]->GetHeight());
			SetActiveTexture(tactical::GLTexture::TEXTURE0);
			m_framebuffers["depthMap"]->Bind();
			glClear(GL_DEPTH_BUFFER_BIT);
			glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		}

		// TODO: start
		void Renderer::ShadowPassEnd()
		{
			m_framebuffers["depthMap"]->Unbind();
			m_shaders["depthMap"]->Disable();
			glClearColor(135.0f / 255.0f, 206.0f / 255.0f, 250.0f / 255.0f, 1.0f);
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

			if (!IsShaderEnabled("basic_light")) EnableShader("basic_light");
			m_shaders["basic_light"]->SetUniform1i("light_type", m_lightType);
		}

		void Renderer::SetPointLightPosition(const glm::vec3& position) 
		{
			m_pointLight.position = position;

			if (!IsShaderEnabled("basic_light")) EnableShader("basic_light");
			m_shaders["basic_light"]->SetUniform3fv("pointLight.position", m_pointLight.position);
		}

		void Renderer::SetSpotLightPosition(const glm::vec3& position)
		{
			m_spotLight.position = position;

			if (!IsShaderEnabled("basic_light")) EnableShader("basic_light");
			m_shaders["basic_light"]->SetUniform3fv("spotLight.position", m_spotLight.position);
		}

		void Renderer::SetSpotLightDirection(const glm::vec3& direction)
		{
			m_spotLight.direction = direction;

			if (!IsShaderEnabled("basic_light")) EnableShader("basic_light");
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

			if (!IsShaderEnabled("basic_light")) EnableShader("basic_light");
			m_shaders["basic_light"]->SetUniformBool("fog_enabled", m_renderFog);
		}

		Shader* Renderer::GetShader(std::string shaderID) 
		{ 
			if (!(m_shaders.find(shaderID) == m_shaders.end()))
				return m_shaders[shaderID];
			else
				return nullptr; // TODO: add error catching
		}

		void Renderer::EnableShader(std::string shaderID)
		{
			m_currentShader = shaderID;
			m_shaders[shaderID]->Enable(); 
		}

		void Renderer::DisableShader(std::string shaderID)
		{
			m_currentShader = "none";
			m_shaders[shaderID]->Disable();
		}
		
		FramebufferTexture* Renderer::GetFramebufferTexture(std::string fbID) 
		{ 
			if (!(m_framebufferTextures.find(fbID) == m_framebufferTextures.end()))
				return m_framebufferTextures[fbID];
			else
				return nullptr; // TODO: add error catching
		}

		void Renderer::ReloadShaders()
		{
			LOG_INFO("Loading shaders...");

			for (auto it = m_shaders.begin(); it != m_shaders.end(); ++it) {
				delete (*it).second;
			}

			m_shaders.clear();

			m_shaders["basic_light"] = new Shader("shaders/vertex.glsl", "shaders/frag.glsl", nullptr);
			m_shaders["normal"] = new Shader("shaders/normal_vertex.glsl", "shaders/normal_frag.glsl", "shaders/normal_geom.glsl");
			m_shaders["picking"] = new Shader("shaders/picking_vert.glsl", "shaders/picking_frag.glsl", nullptr);
			m_shaders["depthMap"] = new Shader("shaders/depth.vert.glsl", "shaders/depth.frag.glsl", nullptr);
			m_shaders["depthDebug"] = new Shader("shaders/depthDebugQuad.vert.glsl", "shaders/depthDebugQuad.frag.glsl", nullptr);

			EnableShader("basic_light");
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

			m_shaders["basic_light"]->SetUniform3fv("pointLight.position", m_pointLight.position);
			m_shaders["basic_light"]->SetUniform4fv("pointLight.color", m_pointLight.color);
			m_shaders["basic_light"]->SetUniform3fv("pointLight.ambient", m_pointLight.ambient);
			m_shaders["basic_light"]->SetUniform3fv("pointLight.diffuse", m_pointLight.diffuse);
			m_shaders["basic_light"]->SetUniform3fv("pointLight.specular", m_pointLight.specular);
			m_shaders["basic_light"]->SetUniform1f("pointLight.constant", m_pointLight.constant);
			m_shaders["basic_light"]->SetUniform1f("pointLight.linear", m_pointLight.linear);
			m_shaders["basic_light"]->SetUniform1f("pointLight.quadratic", m_pointLight.quadratic);

			m_shaders["basic_light"]->SetUniform3fv("spotLight.position", m_spotLight.position);
			m_shaders["basic_light"]->SetUniform4fv("spotLight.color", m_spotLight.color);
			m_shaders["basic_light"]->SetUniform3fv("spotLight.direction", m_spotLight.direction);
			m_shaders["basic_light"]->SetUniform3fv("spotLight.ambient", m_spotLight.ambient);
			m_shaders["basic_light"]->SetUniform3fv("spotLight.diffuse", m_spotLight.diffuse);
			m_shaders["basic_light"]->SetUniform3fv("spotLight.specular", m_spotLight.specular);
			m_shaders["basic_light"]->SetUniform1f("spotLight.constant", m_spotLight.constant);
			m_shaders["basic_light"]->SetUniform1f("spotLight.linear", m_spotLight.linear);
			m_shaders["basic_light"]->SetUniform1f("spotLight.quadratic", m_spotLight.quadratic);
			m_shaders["basic_light"]->SetUniform1f("spotLight.cutOff", m_spotLight.cutOff);
			m_shaders["basic_light"]->SetUniform1f("spotLight.outerCutOff", m_spotLight.outerCutOff);

			m_shaders["basic_light"]->SetUniform1i("shadowMap", 0);

			EnableShader("normal");
			m_shaders["normal"]->SetUniformMat4fv("projection", m_pCamera->GetProjectionMatrix());
			m_shaders["normal"]->SetUniformMat4fv("model", glm::mat4(1.0f));

			EnableShader("picking");
			m_shaders["picking"]->SetUniformMat4fv("projection", m_pCamera->GetProjectionMatrix());
			m_shaders["picking"]->SetUniformMat4fv("model", glm::mat4(1.0f));

			EnableShader("depthMap");
			m_shaders["depthMap"]->SetUniformMat4fv("model", glm::mat4(1.0f));
			//m_shaders["depthMap"]->SetUniform1i("depthMap", (GLint)0);

			EnableShader("depthDebug");
			m_shaders["depthDebug"]->SetUniform1i("depthMap", (GLint)1);
			//m_shaders["depthDebug"]->SetUniform1f("near_plane", (GLfloat)1.0f);
			//m_shaders["depthDebug"]->SetUniform1f("far_plane", (GLfloat)1.0f);

			LOG_INFO("Shaders loaded.");
		}

		
	}
}