#define USE_VT_MODE 1

#include <chrono>
#include "Terminal/Renderer.h"
#if USE_VT_MODE
#include "Terminal/VTOutput.h"
#else
#include "Terminal/WinWriteConsoleOutput.h"
#endif
#include "Terminal/Colors.h"
#include "Engine/VoxelRenderer.h"
#include "World/Camera.h"
#include "Game/Entities/Player.h"
#include "World/ChunkManager.h"
#include "Core/Profiler.h"
#include "Platform/Platform.h"

int main()
{
	Platform::init();

	Renderer renderer;
	renderer.init();

	const float DAY_LENGTH = 30.0f;
	float timeOfDay = .3f;


	Lighting lighting;

#if USE_VT_MODE
	VTOutput vtOutput;
#else
	WinWriteConsoleOutput winOutput;
#endif

	ChunkManager chunkManager;
	chunkManager.setBlockProperties();

	VoxelRenderer voxelRenderer;

	Camera camera;
	camera.fovRadius = camera.fov * 3.14159f / 180.f;
	camera.focalLen = 1.0f / tanf(camera.fovRadius * .5f);
	camera.updateCatheti();

	Player player(camera);
	Vector3 startingPlayerPosition = { 0,200,-2 };
	player.setPosition(startingPlayerPosition);
	player.setRotation(Vector3{ -90, 0, 0 });

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

		timeOfDay += deltaTime / DAY_LENGTH;

		lighting = computeLighting(timeOfDay);

		Color skyColor = (lighting.globalLight >= 4) ? C_SKY : C_SKY_NIGHT;
		renderer.backgroundColor = colorIndex(skyColor, lighting.globalLight);

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
			voxelRenderer.render(renderer, camera, lighting);
		}

		{
			ScopedTimer t(presentMs);
		
#if USE_VT_MODE
			vtOutput.present(renderer.getColorBuffer(), renderer.getTextToPrint(), renderer.getLogicalWidth(), renderer.getLogicalHeight(), renderer.getRealWidth(), renderer.getRealHeight());
#else
			winOutput.present(renderer.getColorBuffer(), renderer.getTextToPrint(), renderer.getLogicalWidth(), renderer.getLogicalHeight(), renderer.getRealWidth(), renderer.getRealHeight());
#endif
		}

		renderer.clear();

		loadChunksAccumulator += loadChunksMs;
		commitReadyChunksAccumulator += commitReadyChunksMs;
		unloadChunksMeshesAccumulator += unloadChunksMeshesMs;
		renderAccumulator += renderMs;
		presentAccumulator += presentMs;
		packAccumulator += renderer.packMs;
		writeAccumulator += renderer.writeMs;

		renderer.queueText(2, 0, "FPS: " + std::to_string(currentFPS), C_WHITE);
		renderer.queueText(2, 1, "chunks: " + std::to_string(chunkManager.chunks.size()), C_WHITE);
		renderer.queueText(2, 2, "pending: " + std::to_string(chunkManager.pendingCoords.size()), C_WHITE);
		renderer.queueText(2, 3, "meshingQueue: " + std::to_string(chunkManager.meshingQueue.size()), C_WHITE);
		renderer.queueText(2, 4, "chunksMeshesByPosition: " + std::to_string(voxelRenderer.chunksMeshesByPosition.size()), C_WHITE);
		renderer.queueText(2, 5, "player position: " + std::to_string(player.getPosition().x) + ", " + std::to_string(player.getPosition().y) + ", " + std::to_string(player.getPosition().z), C_WHITE);
		
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

		renderer.queueText(2, 6, "loadChunksMs: " + std::to_string(loadChunksAverage), C_WHITE);
		renderer.queueText(2, 7, "commitReadyChunksMs: " + std::to_string(commitReadyChunksAverage), C_WHITE);
		renderer.queueText(2, 8, "unloadChunksMeshesMs: " + std::to_string(unloadChunksMeshesAverage), C_WHITE);
		renderer.queueText(2, 9, "renderMs: " + std::to_string(renderAverage), C_WHITE);
		renderer.queueText(2, 10, "packMs: " + std::to_string(packAverage), C_WHITE);
		renderer.queueText(2, 11, "writeMs: " + std::to_string(writeAverage), C_WHITE);
		renderer.queueText(2, 12, "presentMs: " + std::to_string(presentAverage), C_WHITE);
		renderer.queueText(2, 13, "" + std::to_string(renderer.getLogicalWidth()) + "x" + std::to_string(renderer.getLogicalHeight()), C_WHITE);
	
		if (GetKeyState('P') & 0x8000)
			break;
	}

	chunkManager.running = false;
	chunkManager.loadQueueCV.notify_all();
	chunkManager.meshingQueueCV.notify_all();
	for (auto& work : workers) 
		work.join();

	Platform::shutdown();

	return 0;
}