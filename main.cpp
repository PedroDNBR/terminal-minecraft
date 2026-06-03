#include <chrono>
#include "Renderer/Renderer.h"
#include "Engine/VoxelRenderer.h"
#include "World/Camera.h"
#include "Game/Entities/Player.h"
#include "World/ChunkManager.h"

int main()
{
	Renderer renderer(true);
	renderer.backgroundColor = 11;

	ChunkManager chunkManager;
	chunkManager.setBlockProperties();

	VoxelRenderer voxelRenderer;

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

		chunkManager.handleChunkLoad(camera);
		chunkManager.handleChunkUnload(camera);

		voxelRenderer.generateMeshes(chunkManager);
		voxelRenderer.unloadMeshes(chunkManager);

		renderer.queueText(2, 1, L"chunksByPosition: " + std::to_wstring(chunkManager.chunksByPosition.size()), 15);
		renderer.queueText(2, 2, L"pending: " + std::to_wstring(chunkManager.pending.size()), 15);
		renderer.queueText(2, 3, L"meshingQueue: " + std::to_wstring(chunkManager.meshingQueue.size()), 15);
		renderer.queueText(2, 4, L"chunksMeshesByPosition: " + std::to_wstring(voxelRenderer.chunksMeshesByPosition.size()), 15);

		voxelRenderer.render(renderer, camera);

		renderer.present();
	}

}