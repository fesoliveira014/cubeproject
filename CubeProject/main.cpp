#pragma comment(lib, "glew32s.lib")

#include "tactical/Engine.h"

//#define TEST

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
		-1000.0f, 1000.0f);

    // Isometric camera
    tactical::render::IsometricCamera isoCamera(ortho);
    // FPS camera
    tactical::render::FPSCamera fpsCamera(persp, glm::vec3(0.0f, 64.0f, 0.0f), glm::vec3(0.0f, -64.0f, 0.0f));

	isoCamera.LinkTo(window);
	fpsCamera.LinkTo(window);
	
	tactical::render::Camera* activeCamera = &fpsCamera;

    tactical::render::Renderer renderer(activeCamera);

    tactical::ChunkManager chunkManager(&renderer, glm::vec3(1, 1, 1));
    //chunkManager.FillChunks();
    //chunkManager.GenerateWorld();
    chunkManager.FillWithPyramids();

    LOG_INFO("Initializing systems...");

    std::vector<tactical::render::DrawableLine> lines;

    tactical::entity::DrawableBox pickingBox(tactical::math::AABB(glm::vec3(0.0f), glm::vec3(1.0f)));
	//pickingBox.Scale(glm::vec3(1.01f));

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

    // Deltatime
    sf::Clock deltaClock; // Clock to get time between current frame and last frame
    deltaClock.restart();

	bool cameraChanged = false;

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

            if (window.GetEventHandler()->GetKeyboardState()->key_f)
                renderer.ToggleFog();
        }

        window.Clear();

        float deltaTime = deltaClock.restart().asSeconds(); // Get time elapsed since last camera update

		// Pass dt as argument to adjust velocity so that the speed isn't FPS-based
		fpsCamera.Update(deltaTime);
		isoCamera.Update(deltaTime);

        chunkManager.UpdateChunks(activeCamera->GetPosition());
		if (cameraChanged) renderer.SetCamera(activeCamera), cameraChanged = false;
        renderer.Update();

        tactical::math::Ray pickingRay = activeCamera->CastPickingRay(activeCamera->GetPosition(),
            glm::vec2(window.GetEventHandler()->GetMouseState()->mouse_x,
                window.GetEventHandler()->GetMouseState()->mouse_y),
            glm::vec2(window.GetEventHandler()->GetWindowSizeState()->width,
                window.GetEventHandler()->GetWindowSizeState()->height));

        tactical::math::RayCastResult pickingResult = chunkManager.GetRayVoxelIntersection(pickingRay, activeCamera->GetPosition(), 50.0f);

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

        renderer.GetShader("basic_light")->SetUniformMat4fv("view", activeCamera->GetViewMatrix());
        renderer.GetShader("basic_light")->SetUniformMat4fv("projection", activeCamera->GetProjectionMatrix());
        renderer.GetShader("basic_light")->SetUniform3fv("camera_pos", activeCamera->GetPosition());
        chunkManager.Draw("basic_light");

        renderer.GetShader("picking")->Enable();
        renderer.GetShader("picking")->SetUniformMat4fv("view", activeCamera->GetViewMatrix());
        renderer.GetShader("picking")->SetUniformMat4fv("projection", activeCamera->GetProjectionMatrix());
        for (auto line : lines)
            line.Draw(*renderer.GetShader("picking"));

        if (pickingResult.hit) {
            pickingBox.SetPosition(pickingResult.pos);
            pickingBox.Draw(*renderer.GetShader("picking"));
        }

        if (renderer.NormalRendering()) {
            renderer.GetShader("normal")->Enable();
            renderer.GetShader("normal")->SetUniformMat4fv("view", activeCamera->GetViewMatrix());
            renderer.GetShader("normal")->SetUniformMat4fv("projection", activeCamera->GetProjectionMatrix());
            chunkManager.Draw("normal");
        }

        framerate++;
        if (clock.getElapsedTime().asMilliseconds() > 999) {
            window.SetTitle("Viewport: " + glm::to_string(glm::ivec2(window.GetEventHandler()->GetWindowSizeState()->width,
                window.GetEventHandler()->GetWindowSizeState()->height)) +
                " FPS: " + std::to_string(framerate));

            if (pickingResult.hit)
                LOG_INFO("Picking position: " + glm::to_string(pickingBox.GetPosition()));

            clock.restart();
            framerate = 0;
        }

        window.Update();
    }

    LOG_INFO("Finalizing systems...");

    delete logger;

    return 0;
}