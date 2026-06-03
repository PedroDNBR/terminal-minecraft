#pragma once
#include <unordered_map>
#include <memory>
#include "TerrainGenerator.h"
#include "Camera.h"
#include "Chunk/Block.h"
#include "Chunk/ChunkCoord.h"
#include "Chunk/Chunk.h"
#include "../Renderer/Vector.h"
#include <unordered_set>
#include <queue>

class ChunkManager
{
public:
	ChunkManager();

	BlockProperties blockProperties[BlockType::MAX] = {};

	Chunk* getChunk(ChunkCoord coord);
	void setBlockProperties();
	bool isTransparent(Chunk* chunk, Vector3Int position);

	void tick(const Camera& camera);

	int renderDistance = 4;

	std::queue<ChunkCoord> meshingQueue;
	std::unordered_set<ChunkCoord, ChunkCoordHash> pending;
	std::unordered_map<ChunkCoord, std::unique_ptr<Chunk>, ChunkCoordHash> chunksByPosition;

private:
	TerrainGenerator terrainGenerator;


	int lastCameraChunkX = 9999999999999;
	int lastCameraChunkZ = 9999999999999;
};

