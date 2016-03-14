#pragma comment(lib, "glew32.lib")

#include "tactical/Engine.h"
//#define TEST

int main(int argc, char* argv[])
{
	tactical::utils::Logger *logger = tactical::utils::Logger::GetInstance();
	logger->SetConsoleLogging(true);

	tactical::window::Window window(1600, 900, "Game window");

	glm::mat4 persp = glm::perspective(45.0f, window.GetEventHandler()->GetWindowSizeState()->aspectRatio, 0.1f, 1000.0f);

	tactical::render::FPSCamera camera(persp, glm::vec3(-6.0f, 65.0f, 21.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	camera.LinkTo(window);

	tactical::render::Renderer renderer(&camera);

	tactical::ChunkManager chunkManager(&renderer, glm::ivec3(2, 1, 2));
	chunkManager.GenerateWorld();

	LOG << LOGTYPE::LOG_INFO << "Initializing systems...";

	sf::Clock clock;
	clock.restart();

	int previous = clock.getElapsedTime().asMilliseconds();
	int lag = 0;
	int framerate = 0;
		
	while (window.IsOpen() == true) {
		if (window.GetEventHandler()->GetKeyboardState()->key_1)
			renderer.TogglePolygonMode();

		if (window.GetEventHandler()->GetKeyboardState()->key_2)
			renderer.ToggleNormalRendering();

		int current = clock.getElapsedTime().asMilliseconds();
		int elapsed = current - previous;
		previous = current;
		lag += elapsed;

		window.Clear();

		camera.Update();
		chunkManager.UpdateChunks(camera.GetPosition());
		renderer.Update();

		renderer.GetShader("basic_light")->Enable();

		renderer.GetShader("basic_light")->SetUniformMat4fv("view", camera.GetViewMatrix());
		renderer.GetShader("basic_light")->SetUniform3fv("camera_pos", camera.GetPosition());
		chunkManager.Draw("basic_light");

		if (renderer.NormalRendering()) {
			renderer.GetShader("normal")->Enable();
			renderer.GetShader("normal")->SetUniformMat4fv("view", camera.GetViewMatrix());
			chunkManager.Draw("normal");
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


