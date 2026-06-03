#pragma once
#include <vector>
#include <memory>
#include "Chunk/Chunk.h"
#include <unordered_map>
#include "../Renderer/Quad.h"

class TerrainGenerator
{
public:
	std::unique_ptr<Chunk> generateChunkData(ChunkCoord chunkPosition);

	int seed = 1;
	int baseTerrainHeight = 28;

};

