#pragma comment(lib, "glew32.lib")

#include "tactical/Engine.h"

//#define TEST

int main(int argc, char* argv[])
{
	tactical::utils::Logger *logger = tactical::utils::Logger::GetInstance();
	logger->SetConsoleLogging(true);

	tactical::window::Window window(1600, 900, "Game window");

	glm::mat4 persp = glm::perspective(45.0f, window.GetEventHandler()->GetWindowSizeState()->aspectRatio, 0.1f, 1000.0f);
	glm::mat4 ortho = glm::ortho(-400.0f, 400.0f, -400.0f, 400.0f, -1000.0f, 1000.0f);

	tactical::render::FPSCamera camera(persp, glm::vec3(0.0f, 64.0f, 0.0f), glm::vec3(640.0f, 0.0f, 640.0f));
	camera.LinkTo(window);

	tactical::render::Renderer renderer(&camera);

	tactical::ChunkManager chunkManager(&renderer, glm::vec3(20, 1, 20));
	//chunkManager.FillChunks();
	chunkManager.GenerateWorld();
	//chunkManager.FillWithPyramids();

	LOG << LOGTYPE::LOG_INFO << "Initializing systems...";

	std::vector<tactical::render::DrawableLine> lines;

	tactical::render::DrawableBox pickingBox(tactical::math::AABB(glm::vec3(0.0f), glm::vec3(1.0f)));
	tactical::render::DrawableLine mouseRay(glm::vec3(0), glm::vec3(1));
	tactical::render::DrawableLine redAxis(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(-1.0f, -1.0f, 20.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	tactical::render::DrawableLine greenAxis(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(-1.0f, 20.0f, -1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	tactical::render::DrawableLine blueAxis(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(20.0f, -1.0f, -1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

	lines.push_back(redAxis);
	lines.push_back(greenAxis);
	lines.push_back(blueAxis);

	sf::Clock clock;
	clock.restart();
	int framerate = 0;
	
	while (window.IsOpen() == true) {
		if (window.GetEventHandler()->GetKeyEvent()->key_pressed) {
			if (window.GetEventHandler()->GetKeyboardState()->key_1)
				renderer.TogglePolygonMode();

			if (window.GetEventHandler()->GetKeyboardState()->key_2)
				renderer.ToggleNormalRendering();
		}

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

		if (window.GetEventHandler()->GetMouseEvent()->mouse_button_pressed) {
			if (window.GetEventHandler()->GetMouseState()->mouse_button_left) {
				if (pickingResult.hit) {
					glm::vec3 pos = pickingResult.pos;
					if (window.GetEventHandler()->GetKeyboardState()->key_shift) {
						chunkManager.SetVoxel(pos, 0);
					}
					else {
						pos += pickingResult.face;
						chunkManager.SetVoxel(pos, 1);
					}
				}
			}

			if (window.GetEventHandler()->GetMouseState()->mouse_button_middle)
				lines.push_back(tactical::render::DrawableLine(pickingRay.GetOrigin(), 20.0f * pickingRay.GetDirection() + pickingRay.GetOrigin()));

		}

		renderer.GetShader("basic_light")->Enable();

		renderer.GetShader("basic_light")->SetUniformMat4fv("view", camera.GetViewMatrix());
		renderer.GetShader("basic_light")->SetUniform3fv("camera_pos", camera.GetPosition());
		chunkManager.Draw("basic_light");

		renderer.GetShader("picking")->Enable();
		renderer.GetShader("picking")->SetUniformMat4fv("view", camera.GetViewMatrix());
		for (auto line : lines)
			line.Draw(*renderer.GetShader("picking"));

		if (pickingResult.hit) {
			pickingBox.SetPosition(pickingResult.pos);
			pickingBox.Draw(*renderer.GetShader("picking"));
		}

		if (renderer.NormalRendering()) {
			renderer.GetShader("normal")->Enable();
			renderer.GetShader("normal")->SetUniformMat4fv("view", camera.GetViewMatrix());
			chunkManager.Draw("normal");
		}

		framerate++;
		if (clock.getElapsedTime().asMilliseconds() > 999) {
			window.SetTitle("Viewport: " + glm::to_string(glm::ivec2(window.GetEventHandler()->GetWindowSizeState()->width,
				window.GetEventHandler()->GetWindowSizeState()->height)) +
				" FPS: " + std::to_string(framerate));

			if (pickingResult.hit) LOG << LOGTYPE::LOG_INFO << "Picking position: " + glm::to_string(pickingBox.GetPosition());
			clock.restart();
			framerate = 0;
		}

		window.Update();
	}
	
	LOG << LOGTYPE::LOG_INFO << "Finalizing systems...";

	delete logger;

	return 0;
}


