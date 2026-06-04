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
#include "../Renderer/Vector.h"


class ChunkManager
{
public:
	ChunkManager();

	BlockProperties blockProperties[BlockType::MAX] = {};

	Chunk* getChunk(ChunkCoord coord);
	void setBlockProperties();
	bool isTransparent(Chunk* chunk, Vector3Int position);

	void handleChunkLoad(const Camera& camera);
	void handleChunkUnload(const Camera& camera);
	void commitLoadedChunks();

	int renderDistance = 4;

	std::unordered_map<ChunkCoord, std::unique_ptr<Chunk>, ChunkCoordHash> chunksByPosition;
	std::shared_mutex chunksMutex;

	std::unordered_set<ChunkCoord, ChunkCoordHash> pending;
	std::mutex pendingMutex;
	std::condition_variable pendingConditionVariable;

	std::queue<LoadedChunk> readyChunks;
	std::mutex readyChunksMutex;

	std::queue<ChunkCoord> meshingQueue;
	std::mutex meshingQueueMutex;
	std::condition_variable meshingQueueConditionVariable;

	std::vector<ChunkCoord> meshUnload;

	std::atomic<bool> running{ false };
	void chunkLoaderWorker();

private:
	TerrainGenerator terrainGenerator;


	int lastCameraChunkX = 9999999999999;
	int lastCameraChunkZ = 9999999999999;
};

