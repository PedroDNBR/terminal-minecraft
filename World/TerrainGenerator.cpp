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

void TerrainGenerator::generateChunk(ChunkCoord chunkPosition)
{
	chunks.push_back(generateChunkData(chunkPosition));
	chunksByPosition[chunkPosition] = chunks.back().get();
}

bool TerrainGenerator::isTransparent(Chunk* chunk, Vector3Int position)
{
	if (position.y < 0 || position.y >= Chunk::SIZE_Y) return true;

	if (position.x >= 0 && position.x < Chunk::SIZE_X && position.z >= 0 && position.z < Chunk::SIZE_Z)
		return chunk->blocks[position.x][position.y][position.z] == AIR || chunk->blocks[position.x][position.y][position.z] == WATER;

	Chunk* neighborChunk = nullptr;

	if (position.x < 0)
	{
		neighborChunk = chunksByPosition[{ chunk->position.x - 1, chunk->position.z }];
		if (neighborChunk != nullptr)
			return neighborChunk->blocks[position.x + Chunk::SIZE_X][position.y][position.z] == AIR || neighborChunk->blocks[position.x + Chunk::SIZE_X][position.y][position.z] == WATER;
	}
	if (position.x >= Chunk::SIZE_X)
	{
		neighborChunk = chunksByPosition[{ chunk->position.x + 1, chunk->position.z }];
		if (neighborChunk != nullptr)
			return neighborChunk->blocks[position.x - Chunk::SIZE_X][position.y][position.z] == AIR || neighborChunk->blocks[position.x - Chunk::SIZE_X][position.y][position.z] == WATER;
	}
	if (position.z < 0)
	{
		neighborChunk = chunksByPosition[{ chunk->position.x, chunk->position.z - 1 }];
		if (neighborChunk != nullptr)
			return neighborChunk->blocks[position.x][position.y][position.z + Chunk::SIZE_Z] == AIR || neighborChunk->blocks[position.x][position.y][position.z + Chunk::SIZE_Z] == WATER;
	}
	if (position.z >= Chunk::SIZE_Z)
	{
		neighborChunk = chunksByPosition[{ chunk->position.x, chunk->position.z + 1 }];
		if (neighborChunk != nullptr)
			return neighborChunk->blocks[position.x][position.y][position.z - Chunk::SIZE_Z] == AIR || neighborChunk->blocks[position.x][position.y][position.z - Chunk::SIZE_Z] == WATER;
	}

	return true;
}

std::unique_ptr<Chunk> TerrainGenerator::generateChunkData(ChunkCoord chunkPosition)
{
	std::unique_ptr<Chunk> chunk = std::make_unique<Chunk>();
	chunk->position = chunkPosition;
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
