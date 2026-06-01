#pragma once
#include <vector>
#include <memory>
#include "Chunk/Chunk.h"
#include <unordered_map>
#include "../Renderer/Quad.h"

class TerrainGenerator
{
public:
	std::unordered_map<ChunkCoord, Chunk*, ChunkCoordHash> chunksByPosition;
	BlockProperties blockProperties[BlockType::MAX] = {};

	void setBlockProperties();
	void generateChunk(ChunkCoord chunkPosition);

	bool isTransparent(Chunk* chunk, Vector3Int position);

	int seed = 1;
	int baseTerrainHeight = 28;

private:
	std::vector<std::unique_ptr<Chunk>> chunks;
	std::unique_ptr<Chunk> generateChunkData(ChunkCoord chunkPosition);
};

