#pragma once
#include <unordered_map>
#include <memory>
#include <unordered_set>
#include <queue>
#include <mutex>
#include <shared_mutex>
#include "TerrainGenerator.h"
#include "Camera.h"
#include "Chunk/Block.h"
#include "Chunk/ChunkCoord.h"
#include "Chunk/LoadedChunk.h"
#include "Chunk/Chunk.h"
#include "Chunk/ChunkLoadRequest.h"
#include "Chunk/WorldEdits.h"
#include "../Core/Vector.h"
#include "../Core/MPSCQueue.h"

class ChunkManager
{
public:
	int renderDistance = 4;
	int loadDistance = 6;
	int unloadDistance = 16;

	std::priority_queue<
		ChunkLoadRequest,
		std::vector<ChunkLoadRequest>,
		std::greater<ChunkLoadRequest>
	> loadQueue;
	std::mutex loadQueueMutex;
	std::condition_variable loadQueueCV;

	MPSCQueue<std::shared_ptr<Chunk>> loadedChunksQueue;

	std::unordered_map<ChunkCoord, std::shared_ptr<Chunk>, ChunkCoordHash> chunks;
	mutable std::shared_mutex chunksMutex;

	std::atomic<bool> running{ true };

	std::unordered_set<ChunkCoord, ChunkCoordHash> pendingCoords;
	std::mutex pendingMutex;

	std::queue<ChunkCoord> meshingQueue;
	std::mutex meshingQueueMutex;
	std::condition_variable meshingQueueCV;

	std::vector<ChunkCoord> meshUnload;

	WorldEdits edits;

	Chunk* getChunk(ChunkCoord coord);
	std::shared_ptr<Chunk> getChunkSharedPtr(ChunkCoord coord);

	void handleChunkLoad(const Camera& camera);
	void handleChunkUnload(const Camera& camera);
	void commitLoadedChunks();
	void chunkLoaderWorker();

	void requestChunkRebuild(ChunkCoord coord);

private:
	TerrainGenerator terrainGenerator;
};
