#pragma once
#include "Block.h"
#include "../../Renderer/Vector.h"

struct Chunk
{
	static constexpr int SIZE_X = 16;
	static constexpr int SIZE_Y = 64;
	static constexpr int SIZE_Z = 16;

	BlockType blocks[SIZE_X][SIZE_Y][SIZE_Z];

	Vector2Int position;
};