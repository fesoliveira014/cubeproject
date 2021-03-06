#pragma comment(lib, "glew32s.lib")

#include "tactical/Engine.h"
#include "tactical/Common.h"

//#define TEST

std::mutex globalMutex; // global mutex for resolving race conditions problems when using multithreads

int main(int argc, char* argv[])
{
	tactical::utils::Logger *logger = tactical::utils::Logger::GetInstance();
	logger->SetConsoleLogging(true);

	tactical::window::Window window(1600, 900, "Game window");

	glm::mat4 persp = glm::perspective(45.0f, window.GetEventHandler()->GetWindowSizeState()->aspectRatio, 0.1f, 1000.0f);
	glm::mat4 ortho = glm::ortho(
		-window.GetEventHandler()->GetWindowSizeState()->aspectRatio * tactical::render::ISOMETRIC_WIDTH / 2,
		window.GetEventHandler()->GetWindowSizeState()->aspectRatio * tactical::render::ISOMETRIC_WIDTH / 2,
		-tactical::render::ISOMETRIC_HEIGHT / 2,
		tactical::render::ISOMETRIC_HEIGHT / 2,
		-tactical::render::ISOMETRIC_DISTANCE / 2, tactical::render::ISOMETRIC_DISTANCE / 2);

	// Isometric camera
	tactical::render::IsometricCamera isoCamera(ortho);
	// FPS camera
	tactical::render::FPSCamera fpsCamera(persp, glm::vec3(0.0f, 64.0f, 0.0f), glm::vec3(0.0f, -64.0f, 0.0f));

	isoCamera.LinkTo(window);
	fpsCamera.LinkTo(window);

	tactical::render::Camera* activeCamera = &fpsCamera;

	tactical::render::Renderer renderer(activeCamera);
	renderer.LinkTo(window);

	tactical::ChunkManager chunkManager(&renderer, glm::vec3(8, 2, 8));
	//chunkManager.FillChunks();
	chunkManager.GenerateWorld();
	//chunkManager.FillWithPyramids();
	//chunkManager.FillWithPlanes(1);

	LOG_INFO("Initializing systems...");

	std::vector<tactical::render::DrawableLine> lines;

	tactical::entity::DrawableBox pickingBox(tactical::math::AABB(glm::vec3(0.0f), glm::vec3(1.0f)));
	//pickingBox.Scale(glm::vec3(1.01f));

	tactical::entity::Prism testPrism(glm::vec3(10, 10, 10));
	testPrism.Scale(glm::vec3(0.25f));

	tactical::render::DrawableLine mouseRay(glm::vec3(0), glm::vec3(1));
	tactical::render::DrawableLine redAxis(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(-1.0f, -1.0f, 20.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	tactical::render::DrawableLine greenAxis(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(-1.0f, 20.0f, -1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	tactical::render::DrawableLine blueAxis(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(20.0f, -1.0f, -1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

	tactical::render::DrawableLine pickRay(glm::vec3(0), glm::vec3(0), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

	lines.push_back(redAxis); // z axis
	lines.push_back(greenAxis); // y axis
	lines.push_back(blueAxis); // x axis

	sf::Clock clock;
	clock.restart();
	int framerate = 0;

	// Deltatime
	sf::Clock deltaClock; // Clock to get time between current frame and last frame
	deltaClock.restart();
	int lightAngle = 0;

	bool cameraChanged = false;
	bool drawDebugQuad = false;

	tactical::render::Quad<tactical::render::FramebufferTexture> debugQuad;
	debugQuad.SetTexture(renderer.GetFramebufferTexture("depthMap"));

	while (window.IsOpen() == true) {
		if (window.GetEventHandler()->GetKeyEvent()->key_pressed) {
			if (window.GetEventHandler()->GetKeyboardState()->key_1)
				renderer.TogglePolygonMode();

			if (window.GetEventHandler()->GetKeyboardState()->key_2)
				renderer.ToggleNormalRendering();

			if (window.GetEventHandler()->GetKeyboardState()->key_3)
				activeCamera = &fpsCamera, cameraChanged = true;

			if (window.GetEventHandler()->GetKeyboardState()->key_4)
				activeCamera = &isoCamera, cameraChanged = true;

			if (window.GetEventHandler()->GetKeyboardState()->key_5)
				renderer.SelectLightType(0);

			if (window.GetEventHandler()->GetKeyboardState()->key_6)
				renderer.SelectLightType(1);

			if (window.GetEventHandler()->GetKeyboardState()->key_7)
				renderer.SelectLightType(2);

			if (window.GetEventHandler()->GetKeyboardState()->key_f)
				renderer.ToggleFog();

			if (window.GetEventHandler()->GetKeyboardState()->key_backspace)
				renderer.ReloadShaders();

			if (window.GetEventHandler()->GetKeyboardState()->key_k)
				drawDebugQuad = !drawDebugQuad;

		}

		window.Clear();

		float deltaTime = deltaClock.restart().asSeconds(); // Get time elapsed since last camera update

		// Pass dt as argument to adjust velocity so that the speed isn't FPS-based
		activeCamera->Update(deltaTime);
		

		chunkManager.UpdateChunks(activeCamera->GetPosition());
		if (cameraChanged) renderer.SetCamera(activeCamera), cameraChanged = false;
		renderer.Update(deltaTime);

		tactical::math::Ray pickingRay = activeCamera->CastPickingRay(
			glm::vec2(window.GetEventHandler()->GetMouseState()->mouse_x,
				window.GetEventHandler()->GetMouseState()->mouse_y),
			glm::vec2(window.GetEventHandler()->GetWindowSizeState()->width,
				window.GetEventHandler()->GetWindowSizeState()->height));

		testPrism.RotateY(testPrism.GetAngle() + deltaTime);

		float distance = activeCamera->GetActiveCamera() == tactical::render::Camera::ISOMETRIC ? 256 : 128;
		tactical::math::RayCastResult pickingResult = chunkManager.GetRayVoxelIntersection(pickingRay, activeCamera->GetPosition(), distance);

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
		}

		if (renderer.GetLightType() == 0) {
			renderer.ShadowPassStart(deltaTime);
			chunkManager.Draw("depthMap");
			if (pickingResult.hit) {
				testPrism.Draw(*renderer.GetShader("depthMap"));
			}
			renderer.ShadowPassEnd();
		}

		renderer.EnableShader("basic_light");

		renderer.GetShader("basic_light")->SetUniformMat4fv("view", activeCamera->GetViewMatrix());
		renderer.GetShader("basic_light")->SetUniformMat4fv("projection", activeCamera->GetProjectionMatrix());
		renderer.GetShader("basic_light")->SetUniform3fv("camera_pos", activeCamera->GetPosition());
		
		chunkManager.Draw("basic_light");

		renderer.EnableShader("picking");
		renderer.GetShader("picking")->SetUniformMat4fv("view", activeCamera->GetViewMatrix());
		renderer.GetShader("picking")->SetUniformMat4fv("projection", activeCamera->GetProjectionMatrix());

		for (auto line : lines)
			line.Draw(*renderer.GetShader("picking"));

		if (activeCamera->GetActiveCamera() == tactical::render::Camera::ActiveCamera::ISOMETRIC) {
			pickRay.Set(pickingRay.GetOrigin(), pickingRay.GetOrigin() + pickingRay.GetDirection() * 100.0f);
		}

		pickRay.Draw(*renderer.GetShader("picking"));

		if (pickingResult.hit) {
			pickingBox.SetPosition(pickingResult.pos);
			testPrism.SetPosition(pickingResult.pos + glm::vec3(0, 1, 0));

			pickingBox.Draw(*renderer.GetShader("picking"));

			renderer.EnableShader("basic_light");
			testPrism.Draw(*renderer.GetShader("basic_light"));
			renderer.SetPointLightPosition(pickingResult.pos + glm::vec3(0.5f, 0.0f, 0.5f));
			renderer.SetSpotLightPositionAndDirection(pickingResult.pos + glm::vec3(0.5f, 0.0f, 0.5f), activeCamera->GetForwardDirection());
		}

		if (renderer.NormalRendering()) {
			renderer.EnableShader("normal");
			renderer.GetShader("normal")->SetUniformMat4fv("view", activeCamera->GetViewMatrix());
			renderer.GetShader("normal")->SetUniformMat4fv("projection", activeCamera->GetProjectionMatrix());
			chunkManager.Draw("normal");
		}

		if (drawDebugQuad) {
			renderer.EnableShader("depthDebug");
			renderer.SetActiveTexture(tactical::GLTexture::TEXTURE1);
			debugQuad.Draw(*renderer.GetShader("depthDebug"));
			renderer.DisableShader("depthDebug");
			renderer.SetActiveTexture(tactical::GLTexture::TEXTURE0);

		}

		framerate++;
		if (clock.getElapsedTime().asMilliseconds() > 999) {
			lightAngle += 1;
			if (lightAngle / 360 > 0) lightAngle = 0;
			window.SetTitle("Viewport: " + glm::to_string(glm::ivec2(window.GetEventHandler()->GetWindowSizeState()->width,
				window.GetEventHandler()->GetWindowSizeState()->height)) +
				" FPS: " + std::to_string(framerate));

			if (pickingResult.hit) {
				LOG_INFO("Picking position: " + glm::to_string(pickingBox.GetPosition()));
			}

			clock.restart();
			framerate = 0;
		}

		window.Update();
	}

	LOG_INFO("Finalizing systems...");

	delete logger;

	return 0;
}