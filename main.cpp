#include <chrono>
#include "Renderer/Renderer.h"
#include "Engine/VoxelRenderer.h"
#include "World/Camera.h"
#include "Game/Entities/Player.h"
#include "World/TerrainGenerator.h"

int main()
{
	Renderer renderer(true);
	renderer.backgroundColor = 11;
	TerrainGenerator terrainGenerator;
	terrainGenerator.setBlockProperties();
	terrainGenerator.generateChunk({ 0, 0 });
	terrainGenerator.generateChunk({ 1, 0 });
	terrainGenerator.generateChunk({ 0, 1 });
	terrainGenerator.generateChunk({ -1, 0 });
	terrainGenerator.generateChunk({ 0, -1 });
	VoxelRenderer voxelRenderer;
	voxelRenderer.generateChunkMesh(terrainGenerator.chunksByPosition[{0, 0}], terrainGenerator.blockProperties, &terrainGenerator);
	voxelRenderer.generateChunkMesh(terrainGenerator.chunksByPosition[{1, 0}], terrainGenerator.blockProperties, &terrainGenerator);
	voxelRenderer.generateChunkMesh(terrainGenerator.chunksByPosition[{0, 1}], terrainGenerator.blockProperties, &terrainGenerator);
	voxelRenderer.generateChunkMesh(terrainGenerator.chunksByPosition[{-1, 0}], terrainGenerator.blockProperties, &terrainGenerator);
	voxelRenderer.generateChunkMesh(terrainGenerator.chunksByPosition[{0, -1}], terrainGenerator.blockProperties, &terrainGenerator);

	Camera camera;
	camera.pitch = 0;
	camera.fovRadius = camera.fov * 3.14159f / 180.f;
	camera.focalLen = 1.0f / tanf(camera.fovRadius * .5f);
	camera.updateCatheti();

	Player player(camera);
	float testingMovementSpeed = 2.f;
	Vector3 startingPlayerPosition = { 0,0,-2 };
	player.setPosition(startingPlayerPosition);

	float deltaTime = 0.1f;
	auto lastTime = std::chrono::high_resolution_clock::now();
	auto now = std::chrono::high_resolution_clock::now();

	while (true)
	{
		auto now = std::chrono::high_resolution_clock::now();
		deltaTime = std::chrono::duration<float>(now - lastTime).count();
		lastTime = now;
		if (deltaTime > 0.1f) deltaTime = 0.1f;

		if (GetKeyState('W') & 0x8000) player.setPosition(player.getPosition() + player.getCamera().getForward() * testingMovementSpeed * deltaTime);
		if (GetKeyState('S') & 0x8000) player.setPosition(player.getPosition() + player.getCamera().getForward() * -testingMovementSpeed * deltaTime);
		if (GetKeyState('D') & 0x8000) player.setPosition(player.getPosition() + player.getCamera().getRight() * testingMovementSpeed * deltaTime);
		if (GetKeyState('A') & 0x8000) player.setPosition(player.getPosition() + player.getCamera().getRight() * -testingMovementSpeed * deltaTime);

		if (GetKeyState('J') & 0x8000) player.setRotation(player.getRotation() + Vector3{ 0, testingMovementSpeed, 0 } *20*deltaTime);
		if (GetKeyState('L') & 0x8000) player.setRotation(player.getRotation() + Vector3{ 0, -testingMovementSpeed, 0 } *20 *deltaTime);
		if (GetKeyState('I') & 0x8000) player.setRotation(player.getRotation() + Vector3{ testingMovementSpeed, 0, 0 } *20 *deltaTime);
		if (GetKeyState('K') & 0x8000) player.setRotation(player.getRotation() + Vector3{ -testingMovementSpeed, 0, 0 } *20 *deltaTime);


		if (GetKeyState(VK_SPACE) & 0x8000) player.setPosition(player.getPosition() + Vector3{ 0, testingMovementSpeed, 0 } *deltaTime);
		if (GetKeyState(VK_LCONTROL) & 0x8000) player.setPosition(player.getPosition() + Vector3{ 0, -testingMovementSpeed, 0 } *deltaTime);

		renderer.hasWindowResized();

		renderer.clear();

		camera.updateCatheti();

		voxelRenderer.render(renderer, camera);

		renderer.present();
	}

}