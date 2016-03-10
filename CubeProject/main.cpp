#pragma comment(lib, "glew32.lib")

#include "tactical/Engine.h"
//#define TEST

int main(int argc, char* argv[])
{
	tactical::utils::Logger *logger = tactical::utils::Logger::GetInstance();
	logger->SetConsoleLogging(true);

	tactical::window::Window window(1600, 900, "Game window");
#ifdef TEST
	tactical::volume::Chunk chunk(glm::vec3(0.0f, 0.0f, 0.0f), 16, 16);

	tactical::math::PerlinNoise2D noise(0.5, 1.0 / 64.0, 4, 10, 13);
	tactical::math::SimplexNoise simplex(0.5, 8, 1);

	for (tactical::uint i = 0; i < chunk.GetSize(); ++i) {
		for (tactical::uint k = 0; k < chunk.GetSize(); ++k) {
			//LOG << LOGTYPE::LOG_INFO << "noise at " + std::to_string(i) + ", " + std::to_string(k) + ": " + std::to_string(noise.GetHeight(i + chunk.GetPosition().x, k + chunk.GetPosition().z));
			int height = int(chunk.GetMaxHeight() - round(chunk.GetMaxHeight() * noise.GetHeight(i + chunk.GetPosition().x, k + chunk.GetPosition().z)));
			//int height = int(chunk.GetMaxHeight() - round(chunk.GetMaxHeight() * simplex.GetNoise2D(i + chunk.GetPosition().x, k + chunk.GetPosition().z)));
			for (int j = 0; j < height; ++j) {
				chunk.SetVoxel(glm::vec3(i, j, k), 1);
			}
		}
	}
#else
	tactical::ChunkManager chunkManager(glm::ivec3(4, 2, 4));
	//chunkManager.FillChunks();
	chunkManager.GenerateWorld();
#endif
	
	glm::mat4 persp = glm::perspective(45.0f, window.GetEventHandler()->GetWindowSizeState()->aspectRatio, 0.1f, 1000.0f);

	tactical::render::FPSCamera camera(persp, glm::vec3(-6.0f, 65.0f, 21.0f), glm::vec3(0.0f, 0.0f, -1.0f));
	camera.LinkTo(window);

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));

	tactical::render::Shader shader("shaders/vertex.glsl", "shaders/frag.glsl", nullptr);
	tactical::render::Shader normalsShader("shaders/normal_vertex.glsl", "shaders/normal_frag.glsl", "shaders/normal_geom.glsl");

	shader.Enable();

	shader.SetUniformMat4fv("projection", persp);
	shader.SetUniformMat4fv("model", model);
	shader.SetUniform3fv("light_pos", glm::vec3(-6.0f, 70.0f, 21.0f));
	shader.SetUniform3fv("light_color", glm::vec3(1.0f, 1.0f, 1.0f));
	shader.SetUniform3fv("camera_pos", camera.GetPosition());

	shader.Disable();

	normalsShader.Enable();

	normalsShader.SetUniformMat4fv("projection", persp);
	normalsShader.SetUniformMat4fv("model", model);

	normalsShader.Disable();

	LOG << LOGTYPE::LOG_INFO << "Initializing systems...";

	sf::Clock clock;
	clock.restart();

	int previous = clock.getElapsedTime().asMilliseconds();
	int lag = 0;
	int framerate = 0;

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	bool wireframe = false;
	bool showNormals = false;
	
		
	while (window.IsOpen() == true) {
		int current = clock.getElapsedTime().asMilliseconds();
		int elapsed = current - previous;
		previous = current;
		lag += elapsed;

		window.Clear();

		if (window.GetEventHandler()->GetKeyboardState()->key_1)
			wireframe = !wireframe;

		if (window.GetEventHandler()->GetKeyboardState()->key_2)
			showNormals = !showNormals;

		camera.Update();
#ifndef TEST
		chunkManager.UpdateChunks(camera.GetPosition());
#endif

		shader.Enable();

		shader.SetUniformMat4fv("view", camera.GetViewMatrix());
		shader.SetUniform3fv("camera_pos", camera.GetPosition());
		if (wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#ifdef TEST
		chunk.Draw(shader);
#else
		chunkManager.Draw(shader);
#endif
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		shader.Disable();

		if (showNormals) {
			normalsShader.Enable();

			normalsShader.SetUniformMat4fv("view", camera.GetViewMatrix());
#ifdef TEST
			chunk.Draw(normalsShader);
#else
			chunkManager.Draw(normalsShader);
#endif

			normalsShader.Disable();
		}

		framerate++;
		if (clock.getElapsedTime().asMilliseconds() > 999) {
			LOG << LOGTYPE::LOG_INFO << "Camera position: " + glm::to_string(camera.GetPosition());
			std::cout << "FPS: " << framerate << std::endl;
			clock.restart();
			framerate = 0;
		}

		window.Update();
	}
	
	LOG << LOGTYPE::LOG_INFO << "Finalizing systems...";

	delete logger;

	return 0;
}


