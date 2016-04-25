#pragma comment(lib, "glew32.lib")

#include "tactical/Engine.h"
//#define TEST

int main(int argc, char* argv[])
{
	tactical::utils::Logger *logger = tactical::utils::Logger::GetInstance();
	logger->SetConsoleLogging(true);

	tactical::window::Window window(1600, 900, "Game window");

	glm::mat4 persp = glm::perspective(45.0f, window.GetEventHandler()->GetWindowSizeState()->aspectRatio, 0.1f, 1000.0f);

	tactical::render::FPSCamera camera(persp, glm::vec3(0.0f, 64.0f, 0.0f), glm::vec3(640.0f, 0.0f, 640.0f));
	camera.LinkTo(window);

	tactical::render::Renderer renderer(&camera);

	tactical::ChunkManager chunkManager(&renderer, glm::ivec3(20, 1, 20));
	chunkManager.FillChunks();
	//chunkManager.GenerateWorld();

	LOG << LOGTYPE::LOG_INFO << "Initializing systems...";

	sf::Clock clock;
	clock.restart();

	int previous = clock.getElapsedTime().asMilliseconds();
	int lag = 0;
	int framerate = 0;

	tactical::render::DrawableBox pickingBox(tactical::math::AABB(glm::vec3(0.0f), glm::vec3(1.0f)));
		
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

		tactical::math::Ray pickingRay = camera.CastPickingRay(camera.GetPosition(),
										   glm::vec2(window.GetEventHandler()->GetMouseState()->mouse_x,
													 window.GetEventHandler()->GetMouseState()->mouse_y),
										   glm::vec2(window.GetEventHandler()->GetWindowSizeState()->width,
													 window.GetEventHandler()->GetWindowSizeState()->height));

		tactical::math::RayCastResult pickingResult= chunkManager.GetRayVoxelIntersection(pickingRay, camera.GetPosition(), 50.0f);

		renderer.GetShader("basic_light")->Enable();

		renderer.GetShader("basic_light")->SetUniformMat4fv("view", camera.GetViewMatrix());
		renderer.GetShader("basic_light")->SetUniform3fv("camera_pos", camera.GetPosition());
		chunkManager.Draw("basic_light");

		if (pickingResult.hit) {
			pickingBox.SetPosition(glm::floor(pickingResult.pos));
			renderer.GetShader("picking")->Enable();

			renderer.GetShader("picking")->SetUniformMat4fv("view", camera.GetViewMatrix());
			pickingBox.Draw(*renderer.GetShader("picking"));
		}

		if (renderer.NormalRendering()) {
			renderer.GetShader("normal")->Enable();

			renderer.GetShader("normal")->SetUniformMat4fv("view", camera.GetViewMatrix());
			chunkManager.Draw("normal");
		}

		framerate++;
		if (clock.getElapsedTime().asMilliseconds() > 999) {
			LOG << LOGTYPE::LOG_INFO << "Camera position: " + glm::to_string(camera.GetPosition());
			LOG << LOGTYPE::LOG_INFO << "Picking position: " + glm::to_string(pickingBox.GetPosition());
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


