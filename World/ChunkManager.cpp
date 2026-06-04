#include "ChunkManager.h"

ChunkManager::ChunkManager()
{
	terrainGenerator = TerrainGenerator();
}

Chunk* ChunkManager::getChunk(ChunkCoord coord)
{
	auto it = chunksByPosition.find(coord);

	if (it == chunksByPosition.end())
		return nullptr;

	return it->second.get();
}

void ChunkManager::setBlockProperties()
{
	blockProperties[AIR] = { 0, 0, 0, 0, 0, 0 };
	blockProperties[GRASS] = { 6, 6, 5, 5, 10, 5 };
	blockProperties[DIRT] = { 6, 6, 5, 5, 6, 5 };
	blockProperties[STONE] = { 7, 7, 8, 8, 7, 8 };
	blockProperties[WOOD] = { 6, 6, 5, 5, 6, 5 };
	blockProperties[LEAVES] = { 2, 2, 1, 1, 2, 1 };
	blockProperties[WATER] = { 9, 9, 1, 1, 9, 1 };
	blockProperties[SAND] = { 14, 14, 6, 6, 14, 6 };
}

bool ChunkManager::isTransparent(Chunk* chunk, Vector3Int position)
{
	if (position.y < 0 || position.y >= Chunk::SIZE_Y) return true;

	if (position.x >= 0 && position.x < Chunk::SIZE_X && position.z >= 0 && position.z < Chunk::SIZE_Z)
		return chunk->blocks[position.x][position.y][position.z] == AIR || chunk->blocks[position.x][position.y][position.z] == WATER;

	Chunk* neighborChunk = nullptr;

	if (position.x < 0)
	{
		neighborChunk = getChunk({ chunk->position.x - 1, chunk->position.z });
		if (neighborChunk != nullptr)
			return neighborChunk->blocks[position.x + Chunk::SIZE_X][position.y][position.z] == AIR || neighborChunk->blocks[position.x + Chunk::SIZE_X][position.y][position.z] == WATER;
	}
	if (position.x >= Chunk::SIZE_X)
	{
		neighborChunk = getChunk({ chunk->position.x + 1, chunk->position.z });
		if (neighborChunk != nullptr)
			return neighborChunk->blocks[position.x - Chunk::SIZE_X][position.y][position.z] == AIR || neighborChunk->blocks[position.x - Chunk::SIZE_X][position.y][position.z] == WATER;
	}
	if (position.z < 0)
	{
		neighborChunk = getChunk({ chunk->position.x, chunk->position.z - 1 });
		if (neighborChunk != nullptr)
			return neighborChunk->blocks[position.x][position.y][position.z + Chunk::SIZE_Z] == AIR || neighborChunk->blocks[position.x][position.y][position.z + Chunk::SIZE_Z] == WATER;
	}
	if (position.z >= Chunk::SIZE_Z)
	{
		neighborChunk = getChunk({ chunk->position.x, chunk->position.z + 1 });
		if (neighborChunk != nullptr)
			return neighborChunk->blocks[position.x][position.y][position.z - Chunk::SIZE_Z] == AIR || neighborChunk->blocks[position.x][position.y][position.z - Chunk::SIZE_Z] == WATER;
	}

	return true;
}

void ChunkManager::handleChunkLoad(const Camera& camera)
{
	int chunkX = (int)std::floor(
		camera.position.x / Chunk::SIZE_X
	);
	int chunkZ = (int)std::floor(
		camera.position.z / Chunk::SIZE_Z
	);

	if(chunkX == lastCameraChunkX && chunkZ == lastCameraChunkZ)
		return;

	lastCameraChunkX = chunkX;
	lastCameraChunkZ = chunkZ;

	int startX = chunkX - renderDistance;
	int endX = chunkX + renderDistance;
	int startZ = chunkZ - renderDistance;
	int endZ = chunkZ + renderDistance;

	bool anyNew = false;
	{
		for (int x = startX; x <= endX; x++)
			for (int z = startZ; z <= endZ; z++)
			{
				ChunkCoord coord{ x, z };
				if (pending.count(coord) == 0)
				{
					pending.insert(coord);
					anyNew = true;
				}
			}

		if (anyNew)
			pendingConditionVariable.notify_one();
	}
}

void ChunkManager::handleChunkUnload(const Camera& camera)
{
	meshUnload.clear();

	int chunkX = (int)std::floor(
		camera.position.x / Chunk::SIZE_X
	);
	int chunkZ = (int)std::floor(
		camera.position.z / Chunk::SIZE_Z
	);

	std::vector<ChunkCoord> unload;

	{
		std::shared_lock<std::shared_mutex> rlock(chunksMutex);
		for (auto& pair : chunksByPosition)
		{
			ChunkCoord coord = pair.first;
			if (std::abs(coord.x - chunkX) > renderDistance || std::abs(coord.z - chunkZ) > renderDistance)
			{
				unload.push_back(coord);
				meshUnload.push_back(coord);
			}
		}
	}

	if (unload.empty())
		return;

	std::vector<std::unique_ptr<Chunk>> toDestroy;
	toDestroy.reserve(unload.size());
	{
		std::unique_lock<std::shared_mutex> wlock(chunksMutex);
		for (auto& coord : unload)
		{
			auto it = chunksByPosition.find(coord);
			if (it != chunksByPosition.end())
			{
				toDestroy.push_back(std::move(it->second));
				chunksByPosition.erase(it);
			}
		}
	}

	{
		std::lock_guard<std::mutex> plock(pendingMutex);

		for (auto& coord : unload)
			pending.erase(coord);
	}
}

void ChunkManager::commitLoadedChunks()
{
	std::queue<LoadedChunk> local;
	{
		std::lock_guard<std::mutex> lock(readyChunksMutex);
		std::swap(local, readyChunks);
	}

	while (!local.empty())
	{
		LoadedChunk& loadedChunk = local.front();
		{
			std::unique_lock<std::shared_mutex> wlock(chunksMutex);
			chunksByPosition.insert({ loadedChunk.coord, std::move(loadedChunk.chunk) });
		}
		{
			std::lock_guard<std::mutex> lock(meshingQueueMutex);
			meshingQueue.push(loadedChunk.coord);
			meshingQueue.push(loadedChunk.coord + ChunkCoord{ 1,  0 });
			meshingQueue.push(loadedChunk.coord + ChunkCoord{ -1,  0 });
			meshingQueue.push(loadedChunk.coord + ChunkCoord{ 0,  1 });
			meshingQueue.push(loadedChunk.coord + ChunkCoord{ 0, -1 });
		}
		meshingQueueConditionVariable.notify_one();
		local.pop();
	}
}

void ChunkManager::chunkLoaderWorker()
{
	while (running)
	{
		ChunkCoord coord;
		{
			std::unique_lock<std::mutex> lock(pendingMutex);
			pendingConditionVariable.wait(lock, [this] {
				return !pending.empty() || !running;
			});
			if(!running)
				break;

			auto iteration = pending.begin();
			coord = *iteration;
			pending.erase(iteration);
		}

		{
			std::shared_lock<std::shared_mutex> rlock(chunksMutex);
			if(chunksByPosition.count(coord) > 0)
				continue;
		}

		auto chunk = terrainGenerator.generateChunkData(coord);
		
		{
			std::lock_guard<std::mutex> lock(readyChunksMutex);
			readyChunks.push({ coord, std::move(chunk) });
		}
	}
}
