#include "TerrainGenerator.h"
#include "Noise.h"

std::unique_ptr<Chunk> TerrainGenerator::generateChunkData(ChunkCoord chunkPosition)
{
	std::unique_ptr<Chunk> chunk = std::make_unique<Chunk>();
	chunk->position = chunkPosition;
	std::memset(chunk->blocks, AIR, sizeof(chunk->blocks));

	for (int x = 0; x < Chunk::SIZE_X; x++)
	for (int z = 0; z < Chunk::SIZE_Z; z++)
	{
		int worldX = chunkPosition.x * Chunk::SIZE_X + x;
		int worldZ = chunkPosition.z * Chunk::SIZE_Z + z;
		int height = (int)(smoothNoise(worldX * 0.01f, worldZ * 0.01f, seed) * Chunk::SIZE_Y);

		for (int y = 0; y < Chunk::SIZE_Y; y++)
		{
			if(y > height)
				chunk->blocks[x][y][z] = AIR;
			else if (y == height)
				chunk->blocks[x][y][z] = GRASS;
			else if (y < height && y > height - 4)
				chunk->blocks[x][y][z] = DIRT;
			else if (y == 0)
				chunk->blocks[x][y][z] = BED_ROCK;
			else
				chunk->blocks[x][y][z] = STONE;
		}
	}

	return chunk;
}
