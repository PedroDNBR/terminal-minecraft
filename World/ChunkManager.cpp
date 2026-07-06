#include "ChunkManager.h"

ChunkManager::ChunkManager()
{
	terrainGenerator = TerrainGenerator();
}

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

void ChunkManager::setBlockProperties()
{
	blockProperties[AIR] = { 0, 0, 0, 0, 0, 0 };
	blockProperties[GRASS] = { 6, 6, 5, 5, 10, 5 };
	blockProperties[DIRT] = { 6, 6, 5, 5, 6, 5 };
	blockProperties[STONE] = { 7, 7, 8, 8, 7, 8 };
	blockProperties[LOG] = { 6, 6, 5, 5, 6, 5 };
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

bool ChunkManager::isTransparent(Chunk* chunk, Chunk* negativeXNeighbour, Chunk* positiveXNeighbour, Chunk* negativeZNeighbour, Chunk* positiveZNeighbour, Vector3Int position)
{
	if (position.y < 0 || position.y >= Chunk::SIZE_Y) return true;

	if (position.x >= 0 && position.x < Chunk::SIZE_X && position.z >= 0 && position.z < Chunk::SIZE_Z)
		return chunk->blocks[position.x][position.y][position.z] == AIR || chunk->blocks[position.x][position.y][position.z] == WATER;

	if (position.x < 0)
		return negativeXNeighbour == nullptr || negativeXNeighbour->blocks[position.x + Chunk::SIZE_X][position.y][position.z] == AIR || negativeXNeighbour->blocks[position.x + Chunk::SIZE_X][position.y][position.z] == WATER;
	
	if (position.x >= Chunk::SIZE_X)
		return positiveXNeighbour == nullptr || positiveXNeighbour->blocks[position.x - Chunk::SIZE_X][position.y][position.z] == AIR || positiveXNeighbour->blocks[position.x - Chunk::SIZE_X][position.y][position.z] == WATER;
	
	if (position.z < 0)
		return negativeZNeighbour == nullptr || negativeZNeighbour->blocks[position.x][position.y][position.z + Chunk::SIZE_Z] == AIR || negativeZNeighbour->blocks[position.x][position.y][position.z + Chunk::SIZE_Z] == WATER;
	
	if (position.z >= Chunk::SIZE_Z)
		return positiveZNeighbour == nullptr || positiveZNeighbour->blocks[position.x][position.y][position.z - Chunk::SIZE_Z] == AIR || positiveZNeighbour->blocks[position.x][position.y][position.z - Chunk::SIZE_Z] == WATER;

	return true;
}

bool ChunkManager::isWater(Chunk* chunk, Vector3Int position)
{
	if (position.y < 0 || position.y >= Chunk::SIZE_Y) return true;

	if (position.x >= 0 && position.x < Chunk::SIZE_X && position.z >= 0 && position.z < Chunk::SIZE_Z)
		return chunk->blocks[position.x][position.y][position.z] == WATER;

	Chunk* neighborChunk = nullptr;

	if (position.x < 0)
	{
		neighborChunk = getChunk({ chunk->position.x - 1, chunk->position.z });
		if (neighborChunk != nullptr)
			return neighborChunk->blocks[position.x + Chunk::SIZE_X][position.y][position.z] == WATER;
	}
	if (position.x >= Chunk::SIZE_X)
	{
		neighborChunk = getChunk({ chunk->position.x + 1, chunk->position.z });
		if (neighborChunk != nullptr)
			return neighborChunk->blocks[position.x - Chunk::SIZE_X][position.y][position.z] == WATER;
	}
	if (position.z < 0)
	{
		neighborChunk = getChunk({ chunk->position.x, chunk->position.z - 1 });
		if (neighborChunk != nullptr)
			return neighborChunk->blocks[position.x][position.y][position.z + Chunk::SIZE_Z] == WATER;
	}
	if (position.z >= Chunk::SIZE_Z)
	{
		neighborChunk = getChunk({ chunk->position.x, chunk->position.z + 1 });
		if (neighborChunk != nullptr)
			return neighborChunk->blocks[position.x][position.y][position.z - Chunk::SIZE_Z] == WATER;
	}

	return true;
}

bool ChunkManager::isWater(Chunk* chunk, Chunk* negativeXNeighbour, Chunk* positiveXNeighbour, Chunk* negativeZNeighbour, Chunk* positiveZNeighbour, Vector3Int position)
{
	if (position.y < 0 || position.y >= Chunk::SIZE_Y) return true;

	if (position.x >= 0 && position.x < Chunk::SIZE_X && position.z >= 0 && position.z < Chunk::SIZE_Z)
		return chunk->blocks[position.x][position.y][position.z] == WATER;

	if (position.x < 0)
			return negativeXNeighbour == nullptr || negativeXNeighbour->blocks[position.x + Chunk::SIZE_X][position.y][position.z] == WATER;

	if (position.x >= Chunk::SIZE_X)
			return positiveXNeighbour == nullptr || positiveXNeighbour->blocks[position.x - Chunk::SIZE_X][position.y][position.z] == WATER;

	if (position.z < 0)
			return negativeZNeighbour == nullptr || negativeZNeighbour->blocks[position.x][position.y][position.z + Chunk::SIZE_Z] == WATER;

	if (position.z >= Chunk::SIZE_Z)
			return positiveZNeighbour == nullptr || positiveZNeighbour->blocks[position.x][position.y][position.z - Chunk::SIZE_Z] == WATER;

	return true;
}

bool ChunkManager::isAir(Chunk* chunk, Vector3Int position)
{
	if (position.y < 0 || position.y >= Chunk::SIZE_Y) return true;

	if (position.x >= 0 && position.x < Chunk::SIZE_X && position.z >= 0 && position.z < Chunk::SIZE_Z)
		return chunk->blocks[position.x][position.y][position.z] == AIR;

	Chunk* neighborChunk = nullptr;

	if (position.x < 0)
	{
		neighborChunk = getChunk({ chunk->position.x - 1, chunk->position.z });
		if (neighborChunk != nullptr)
			return neighborChunk->blocks[position.x + Chunk::SIZE_X][position.y][position.z] == AIR;
	}
	if (position.x >= Chunk::SIZE_X)
	{
		neighborChunk = getChunk({ chunk->position.x + 1, chunk->position.z });
		if (neighborChunk != nullptr)
			return neighborChunk->blocks[position.x - Chunk::SIZE_X][position.y][position.z] == AIR;
	}
	if (position.z < 0)
	{
		neighborChunk = getChunk({ chunk->position.x, chunk->position.z - 1 });
		if (neighborChunk != nullptr)
			return neighborChunk->blocks[position.x][position.y][position.z + Chunk::SIZE_Z] == AIR;
	}
	if (position.z >= Chunk::SIZE_Z)
	{
		neighborChunk = getChunk({ chunk->position.x, chunk->position.z + 1 });
		if (neighborChunk != nullptr)
			return neighborChunk->blocks[position.x][position.y][position.z - Chunk::SIZE_Z] == AIR;
	}

	return true;
}

bool ChunkManager::isAir(Chunk* chunk, Chunk* negativeXNeighbour, Chunk* positiveXNeighbour, Chunk* negativeZNeighbour, Chunk* positiveZNeighbour, Vector3Int position)
{
	if (position.y < 0 || position.y >= Chunk::SIZE_Y) return true;

	if (position.x >= 0 && position.x < Chunk::SIZE_X && position.z >= 0 && position.z < Chunk::SIZE_Z)
		return chunk->blocks[position.x][position.y][position.z] == AIR;


	if (position.x < 0)
		return negativeXNeighbour == nullptr || negativeXNeighbour->blocks[position.x + Chunk::SIZE_X][position.y][position.z] == AIR;

	if (position.x >= Chunk::SIZE_X)
		return positiveXNeighbour == nullptr || positiveXNeighbour->blocks[position.x - Chunk::SIZE_X][position.y][position.z] == AIR;

	if (position.z < 0)
		return negativeZNeighbour == nullptr || negativeZNeighbour->blocks[position.x][position.y][position.z + Chunk::SIZE_Z] == AIR;

	if (position.z >= Chunk::SIZE_Z)
		return positiveZNeighbour == nullptr || positiveZNeighbour->blocks[position.x][position.y][position.z - Chunk::SIZE_Z] == AIR;

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
		for (const auto& [coord, chunks] : chunks)
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

		auto chunk = terrainGenerator.generateChunkData(request.coord);

		{
			std::lock_guard<std::mutex> lock(pendingMutex);
			pendingCoords.erase(request.coord);
		}

		loadedChunksQueue.push(std::move(chunk));
	}
}
