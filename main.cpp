#include <chrono>
#include "Renderer/Renderer.h"
#include "Engine/VoxelEngine.h"
#include "World/Camera.h"
#include "Game/Entities/Player.h"

int main()
{
	Renderer renderer(80, 50);
	VoxelEngine voxelEngine;
	Camera camera;
	camera.pitch = 0;

	camera.fovRadius = camera.fov * 3.14159f / 180.f;
	camera.focalLen = 1.0f / tanf(camera.fovRadius * .5f);

	camera.updateCatheti();

	Player player(camera);

	float deltaTime = 0.1f;
	auto lastTime = std::chrono::high_resolution_clock::now();
	auto now = std::chrono::high_resolution_clock::now();

	float testingMovementSpeed = 5.f;

	while (true)
	{
		auto now = std::chrono::high_resolution_clock::now();
		deltaTime = std::chrono::duration<float>(now - lastTime).count();
		lastTime = now;
		if (deltaTime > 0.1f) deltaTime = 0.1f;

		if (GetKeyState('W') & 0x8000) player.setPosition(player.getPosition() + Vector3{ 0, 0, testingMovementSpeed } *deltaTime);
		if (GetKeyState('S') & 0x8000) player.setPosition(player.getPosition() + Vector3{ 0, 0, -testingMovementSpeed } *deltaTime);
		if (GetKeyState('D') & 0x8000) player.setPosition(player.getPosition() + Vector3{ testingMovementSpeed, 0, 0 } *deltaTime);
		if (GetKeyState('A') & 0x8000) player.setPosition(player.getPosition() + Vector3{ -testingMovementSpeed, 0, 0 } *deltaTime);
		if (GetKeyState(VK_SPACE) & 0x8000) player.setPosition(player.getPosition() + Vector3{ 0, testingMovementSpeed, 0 } *deltaTime);
		if (GetKeyState(VK_LCONTROL) & 0x8000) player.setPosition(player.getPosition() + Vector3{ 0, -testingMovementSpeed, 0 } *deltaTime);

		renderer.hasWindowResized();

		renderer.clear();

		camera.updateCatheti();

		voxelEngine.render(renderer, camera);

		renderer.present();
	}

}