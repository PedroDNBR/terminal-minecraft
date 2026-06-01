#include "TerrainGenerator.h"

void TerrainGenerator::setBlockProperties()
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

void TerrainGenerator::generateChunk(int chunkX, int chunkY)
{
	chunks.push_back(generateChunkData(chunkX, chunkY));
	chunksByPosition[{chunkX, chunkY}] = chunks.back().get();
}

std::unique_ptr<Chunk> TerrainGenerator::generateChunkData(int chunkX, int chunkY)
{
	std::unique_ptr<Chunk> chunk = std::make_unique<Chunk>();
	chunk->position = Vector2Int{ chunkX, chunkY };
	std::memset(chunk->blocks, AIR, sizeof(chunk->blocks));

	for (int x = 0; x < Chunk::SIZE_X; x++)
	for (int z = 0; z < Chunk::SIZE_Z; z++)
	{
		chunk->blocks[x][3][z] = GRASS;
		chunk->blocks[x][2][z] = DIRT;
		chunk->blocks[x][1][z] = STONE;
		chunk->blocks[x][0][z] = BED_ROCK;
	}

	return chunk;
}
