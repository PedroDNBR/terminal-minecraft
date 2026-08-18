#pragma once
#include "Block.h"
#include "ChunkCoord.h"

struct Chunk
{
	static constexpr int SIZE_X = 16;
	static constexpr int SIZE_Z = 16;
	static constexpr int SIZE_Y = 64;

	BlockType blocks[SIZE_X][SIZE_Z][SIZE_Y];

	ChunkCoord position;

	uint8_t heightMap[SIZE_X][SIZE_Z];
	uint8_t skyLight[SIZE_X][SIZE_Z][SIZE_Y];
};