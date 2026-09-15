#include "ChunkManager.h"

Chunk* ChunkManager::getChunk(ChunkCoord coord)
{
	auto it = chunks.find(coord);

	if (it == chunks.end())
		return nullptr;

	return it->second.get();
}

std::shared_ptr<Chunk> ChunkManager::getChunkSharedPtr(ChunkCoord coord)
{
	auto it = chunks.find(coord);

	if (it == chunks.end())
		return nullptr;

	return it->second;
}

void ChunkManager::handleChunkLoad(const Camera& camera)
{
	int chunkX = (int)std::floor(
		camera.position.x / Chunk::SIZE_X
	);
	int chunkZ = (int)std::floor(
		camera.position.z / Chunk::SIZE_Z
	);

	for (int distanceZ = -loadDistance; distanceZ <= loadDistance; distanceZ++)
	for (int distanceX = -loadDistance; distanceX <= loadDistance; distanceX++)
	{
		ChunkCoord coord{ chunkX + distanceX, chunkZ + distanceZ };
		float distanceSq = (float)(distanceX * distanceX + distanceZ * distanceZ);

		{
			std::shared_lock<std::shared_mutex> rlock(chunksMutex);
			if (chunks.count(coord)) continue;
		}
		{
			std::lock_guard<std::mutex> lock(pendingMutex);
			if (pendingCoords.count(coord)) continue;
			pendingCoords.insert(coord);
		}

		{
			std::lock_guard<std::mutex> lock(loadQueueMutex);
			loadQueue.push({ coord, distanceSq });
		}
		loadQueueCV.notify_one();
	}
}

void ChunkManager::handleChunkUnload(const Camera& camera)
{
	int chunkX = (int)floor(camera.position.x / Chunk::SIZE_X);
	int chunkZ = (int)floor(camera.position.z / Chunk::SIZE_Z);

	std::vector<ChunkCoord> toRemove;

	{
		std::shared_lock<std::shared_mutex> rlock(chunksMutex);
		for (const auto& [coord, loadedChunk] : chunks)
		{
			int distanceX = coord.x - chunkX;
			int distanceZ = coord.z - chunkZ;
			if (distanceX * distanceX + distanceZ * distanceZ > unloadDistance * unloadDistance)
				toRemove.push_back(coord);
		}
	}

	if (!toRemove.empty())
	{
		std::unique_lock<std::shared_mutex> wlock(chunksMutex);
		for (const auto& coord : toRemove)
		{
			meshUnload.push_back(coord);
			chunks.erase(coord);
		}
	}
}

void ChunkManager::commitLoadedChunks()
{
	std::shared_ptr<Chunk> chunk;
	while (loadedChunksQueue.pop(chunk))
	{
		ChunkCoord coord = chunk->position;

		std::vector<ChunkCoord> neighborsToRemesh;
		{
			std::unique_lock<std::shared_mutex> wlock(chunksMutex);
			chunks[coord] = std::move(chunk);

			const int dx[] = { 1, -1, 0, 0 };
			const int dz[] = { 0, 0, 1, -1 };
			for (int i = 0; i < 4; i++)
			{
				ChunkCoord nc = { coord.x + dx[i], coord.z + dz[i] };
				if (chunks.count(nc))
					neighborsToRemesh.push_back(nc);
			}
		}

		{
			std::lock_guard<std::mutex> lock(meshingQueueMutex);
			meshingQueue.push(coord);
			for (const ChunkCoord& nc : neighborsToRemesh)
				meshingQueue.push(nc);
		}
		meshingQueueCV.notify_all();
	}
}

void ChunkManager::chunkLoaderWorker()
{
	while (running)
	{
		ChunkLoadRequest request;

		{
			std::unique_lock<std::mutex> lock(loadQueueMutex);
			loadQueueCV.wait(lock, [&] {
				return !loadQueue.empty() || !running;
			});
			if (!running)
				break;
			request = loadQueue.top();
			loadQueue.pop();
		}

		ChunkModifications chunkModifications = edits.copyFor(request.coord);

		auto chunk = terrainGenerator.generateChunkData(request.coord, chunkModifications);

		{
			std::lock_guard<std::mutex> lock(pendingMutex);
			pendingCoords.erase(request.coord);
		}

		loadedChunksQueue.push(std::move(chunk));
	}
}

void ChunkManager::requestChunkRebuild(ChunkCoord coord)
{
	{
		std::lock_guard<std::mutex> lock(pendingMutex);
		pendingCoords.insert(coord);
	}
	{
		std::lock_guard<std::mutex> lock(loadQueueMutex);
		loadQueue.push({ coord, 0.0f });
	}
	loadQueueCV.notify_one();
}
