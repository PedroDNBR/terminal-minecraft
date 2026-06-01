#pragma once
#include <vector>
#include <memory>
#include "Chunk/Chunk.h"
#include <unordered_map>
#include "../Renderer/Quad.h"
class TerrainGenerator
{
public:
	std::unordered_map<Vector2Int, Chunk*, Vector2IntHash> chunksByPosition;
	BlockProperties blockProperties[BlockType::MAX] = {};

	void setBlockProperties();
	void generateChunk(int chunkX, int chunkY);

private:
	std::vector<std::unique_ptr<Chunk>> chunks;
	std::unique_ptr<Chunk> generateChunkData(int chunkX, int chunkY);
};

