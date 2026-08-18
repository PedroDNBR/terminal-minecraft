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
#include "../Core/Vector.h"
#include "../Core/MPSCQueue.h"
#include "../Terminal/Colors.h"


class ChunkManager
{
public:
	ChunkManager();

	int renderDistance = 4;
	int loadDistance = 6;
	int unloadDistance = 16;
	static constexpr BlockProperties BLOCK_PROPERTIES[BlockType::BLOCK_MAX] = {
		/*B_AIR] = */{ Color::C_BLACK, Color::C_BLACK, Color::C_BLACK, Color::C_BLACK, Color::C_BLACK, Color::C_BLACK },
		/*B_GRASS] = */{ Color::C_DIRT, Color::C_DIRT, Color::C_DIRT, Color::C_DIRT, Color::C_GRASS, Color::C_DIRT },
		/*B_DIRT] = */{ Color::C_DIRT, Color::C_DIRT, Color::C_DIRT, Color::C_DIRT, Color::C_DIRT, Color::C_DIRT },
		/*B_STONE] = */{ Color::C_STONE, Color::C_STONE, Color::C_STONE, Color::C_STONE, Color::C_STONE, Color::C_STONE },
		/*B_LOG] = */{ Color::C_LOG, Color::C_LOG, Color::C_LOG, Color::C_LOG, Color::C_LOG, Color::C_LOG },
		/*B_LEAVES] = */{ Color::C_LEAVES, Color::C_LEAVES, Color::C_LEAVES, Color::C_LEAVES, Color::C_LEAVES, Color::C_LEAVES },
		/*B_WATER] = */{ Color::C_WATER, Color::C_WATER, Color::C_WATER, Color::C_WATER, Color::C_WATER, Color::C_WATER },
		/*B_SAND] = */{ Color::C_SAND, Color::C_SAND, Color::C_SAND, Color::C_SAND, Color::C_SAND, Color::C_SAND },
		/*B_CACTUS] = */{ Color::C_CACTUS, Color::C_CACTUS, Color::C_CACTUS, Color::C_CACTUS, Color::C_CACTUS, Color::C_CACTUS }
	};

	static constexpr uint8_t OPACITY[BLOCK_MAX] = {
		/*B_AIR =*/ 0,
		/*B_GRASS =*/ 7,
		/*B_DIRT =*/ 7,
		/*B_STONE =*/ 7,
		/*B_LOG =*/ 7,
		/*B_LEAVES =*/ 2,
		/*B_WATER =*/ 1,
		/*B_SAND =*/ 7,
		/*B_CACTUS =*/ 7,
		/*B_BEDROCK =*/ 7
	};

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

	Chunk* getChunk(ChunkCoord coord);
	std::shared_ptr<Chunk> getChunkSharedPtr(ChunkCoord coord);

	void setBlockProperties();
	bool isTransparent(Chunk* chunk, Vector3Int position);
	bool isTransparent(Chunk* chunk ,Chunk* negativeXNeighbour, Chunk* positiveXNeighbour, Chunk* negativeZNeighbour, Chunk* positiveZNeighbour, Vector3Int position);
	bool isWater(Chunk* chunk, Vector3Int position);
	bool isWater(Chunk* chunk, Chunk* negativeXNeighbour, Chunk* positiveXNeighbour, Chunk* negativeZNeighbour, Chunk* positiveZNeighbour, Vector3Int position);
	bool isAir(Chunk* chunk, Vector3Int position);
	bool isAir(Chunk* chunk, Chunk* negativeXNeighbour, Chunk* positiveXNeighbour, Chunk* negativeZNeighbour, Chunk* positiveZNeighbour, Vector3Int position);
	uint8_t getNeighbourLight(Chunk* chunk, Chunk* negativeXNeighbour, Chunk* positiveXNeighbour, Chunk* negativeZNeighbour, Chunk* positiveZNeighbour, Vector3Int position);

	void handleChunkLoad(const Camera& camera);
	void handleChunkUnload(const Camera& camera);
	void commitLoadedChunks();
	void chunkLoaderWorker();

private:
	TerrainGenerator terrainGenerator;

	int lastCameraChunkX = 9999999999999;
	int lastCameraChunkZ = 9999999999999;
};

