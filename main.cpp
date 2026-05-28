#include <chrono>
#include "Renderer/Renderer.h"
#include "Engine/VoxelEngine.h"
#include "World/Camera.h"

int main()
{
	Renderer renderer(80, 50);
	VoxelEngine voxelEngine;
	Camera camera;

	camera.pitch = 0;

	camera.fovRadius = camera.fov * 3.14159f / 180.f;
	camera.focalLen = 1.0f / tanf(camera.fovRadius * .5f);

	camera.updateCatheti();

	float deltaTime = 0.1f;
	auto lastTime = std::chrono::high_resolution_clock::now();
	auto now = std::chrono::high_resolution_clock::now();

	while (true)
	{
		auto now = std::chrono::high_resolution_clock::now();
		deltaTime = std::chrono::duration<float>(now - lastTime).count();
		lastTime = now;
		if (deltaTime > 0.1f) deltaTime = 0.1f;

		renderer.hasWindowResized();

		renderer.clear();

		camera.updateCatheti();

		voxelEngine.render(renderer, camera);

		renderer.present();
	}

}