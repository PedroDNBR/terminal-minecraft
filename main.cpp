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
	Vector3 startingPlayerPosition = { 0,32,-2 };
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

		player.tick(deltaTime);

		renderer.hasWindowResized();

		renderer.clear();

		camera.updateCatheti();

		voxelRenderer.render(renderer, camera);

		renderer.present();
	}

}