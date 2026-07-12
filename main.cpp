#include <chrono>
#include "Terminal/Renderer.h"
#include "Terminal/Colors.h"
#include "Engine/VoxelRenderer.h"
#include "World/Camera.h"
#include "Game/Entities/Player.h"
#include "World/ChunkManager.h"
#include "Core/Profiler.h"

int main()
{
	Renderer renderer(true);
	renderer.backgroundColor = BRIGHT_CYAN;

	ChunkManager chunkManager;
	chunkManager.setBlockProperties();

	VoxelRenderer voxelRenderer;

	Camera camera;
	camera.pitch = -90 * 3.14159f / 180;
	camera.fovRadius = camera.fov * 3.14159f / 180.f;
	camera.focalLen = 1.0f / tanf(camera.fovRadius * .5f);
	camera.updateCatheti();

	Player player(camera);
	Vector3 startingPlayerPosition = { 0,200,-2 };
	player.setPosition(startingPlayerPosition);

	std::vector<std::thread> workers;
	int threads = std::thread::hardware_concurrency();
	if (threads <= 2) 
		return 1;
	if (threads == 3)
	{
		workers.emplace_back(([&] {chunkManager.chunkLoaderWorker(); }));
		workers.emplace_back(([&] {voxelRenderer.meshBuilderWorker(chunkManager); }));
	}
	else
	{
		workers.emplace_back(([&] {chunkManager.chunkLoaderWorker(); }));
		workers.emplace_back(([&] {chunkManager.chunkLoaderWorker(); }));
		workers.emplace_back(([&] {voxelRenderer.meshBuilderWorker(chunkManager); }));
		workers.emplace_back(([&] {voxelRenderer.meshBuilderWorker(chunkManager); }));
	}

	chunkManager.running = true;

	float deltaTime = 0.1f;
	auto lastTime = std::chrono::high_resolution_clock::now();
	auto now = std::chrono::high_resolution_clock::now();

	auto fpsWinStart = std::chrono::high_resolution_clock::now();
	int  fpsCounter = 0, currentFPS = 0;

	double renderAccumulator = 0;
	double presentAccumulator = 0;
	double packAccumulator = 0;
	double writeAccumulator = 0;
	double loadChunksAccumulator = 0;
	double commitReadyChunksAccumulator = 0;
	double unloadChunksMeshesAccumulator = 0;

	double renderAverage = 0;
	double presentAverage = 0;
	double packAverage = 0;
	double writeAverage = 0;
	double loadChunksAverage = 0;
	double commitReadyChunksAverage = 0;
	double unloadChunksMeshesAverage = 0;


	while (true)
	{
		auto now = std::chrono::high_resolution_clock::now();
		deltaTime = std::chrono::duration<float>(now - lastTime).count();
		lastTime = now;
		if (deltaTime > 0.1f) deltaTime = 0.1f;

		double loadChunksMs = 0;
		double commitReadyChunksMs = 0;
		double unloadChunksMeshesMs = 0;

		double renderMs = 0;
		double presentMs = 0;

		player.tick(deltaTime);

		if(fpsCounter == 0)
			renderer.hasWindowResized();

		camera.updateCatheti();

		chunkManager.handleChunkLoad(camera);
		chunkManager.handleChunkUnload(camera);

		{ 
			ScopedTimer t(loadChunksMs);
			chunkManager.commitLoadedChunks();
		}
		{
			ScopedTimer t(commitReadyChunksMs);
			voxelRenderer.commitReadyMeshes();
		}
		{ 
			ScopedTimer t(unloadChunksMeshesMs);
			voxelRenderer.unloadMeshes(chunkManager);
		}
		

		{
			ScopedTimer t(renderMs);
			voxelRenderer.render(renderer, camera);
		}

		{
			ScopedTimer t(presentMs);
			renderer.present();
		}

		renderer.clear();

		loadChunksAccumulator += loadChunksMs;
		commitReadyChunksAccumulator += commitReadyChunksMs;
		unloadChunksMeshesAccumulator += unloadChunksMeshesMs;
		renderAccumulator += renderMs;
		presentAccumulator += presentMs;
		packAccumulator += renderer.packMs;
		writeAccumulator += renderer.writeMs;

		renderer.queueText(2, 0, L"FPS: " + std::to_wstring(currentFPS), 15);
		renderer.queueText(2, 1, L"chunks: " + std::to_wstring(chunkManager.chunks.size()), 15);
		renderer.queueText(2, 2, L"pending: " + std::to_wstring(chunkManager.pendingCoords.size()), 15);
		renderer.queueText(2, 3, L"meshingQueue: " + std::to_wstring(chunkManager.meshingQueue.size()), 15);
		renderer.queueText(2, 4, L"chunksMeshesByPosition: " + std::to_wstring(voxelRenderer.chunksMeshesByPosition.size()), 15);
		
		fpsCounter++;
		auto  fpsNow = std::chrono::high_resolution_clock::now();
		float fpsElapsed = std::chrono::duration<float>(fpsNow - fpsWinStart).count();
		if (fpsElapsed >= 1.0f) {
			currentFPS = fpsCounter;

			loadChunksAverage = loadChunksAccumulator / fpsCounter;
			commitReadyChunksAverage = commitReadyChunksAccumulator / fpsCounter;
			unloadChunksMeshesAverage = unloadChunksMeshesAccumulator / fpsCounter;
			renderAverage = renderAccumulator / fpsCounter;
			packAverage = packAccumulator / fpsCounter;
			writeAverage = writeAccumulator / fpsCounter;
			presentAverage = presentAccumulator / fpsCounter;

			loadChunksAccumulator = 0;
			commitReadyChunksAccumulator = 0;
			unloadChunksMeshesAccumulator = 0;
			renderAccumulator = 0;
			presentAccumulator = 0;
			packAccumulator = 0;
			writeAccumulator = 0;
			fpsCounter = 0;

			fpsWinStart = fpsNow;
		}

		renderer.queueText(2, 6, L"loadChunksMs: " + std::to_wstring(loadChunksAverage), 15);
		renderer.queueText(2, 7, L"commitReadyChunksMs: " + std::to_wstring(commitReadyChunksAverage), 15);
		renderer.queueText(2, 8, L"unloadChunksMeshesMs: " + std::to_wstring(unloadChunksMeshesAverage), 15);
		renderer.queueText(2, 9, L"renderMs: " + std::to_wstring(renderAverage), 15);
		renderer.queueText(2, 10, L"packMs: " + std::to_wstring(packAverage), 15);
		renderer.queueText(2, 11, L"writeMs: " + std::to_wstring(writeAverage), 15);
		renderer.queueText(2, 12, L"presentMs: " + std::to_wstring(presentAverage), 15);
		renderer.queueText(2, 13, L"" + std::to_wstring(renderer.getLogicalWidth()) + L"x" + std::to_wstring(renderer.getLogicalHeight()), 15);
	
		if (GetKeyState('P') & 0x8000)
			break;
	}

	chunkManager.running = false;
	chunkManager.loadQueueCV.notify_all();
	chunkManager.meshingQueueCV.notify_all();
	for (auto& work : workers) 
		work.join();
	return 0;
}