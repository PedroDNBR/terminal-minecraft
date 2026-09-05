#pragma once
#include "Colunm.h"
#include "Chunk.h"

struct ColunmGrid {
	static constexpr int MARGIN = 2;

	static constexpr int GRID_X = Chunk::SIZE_X + 2 * MARGIN + 1;
	static constexpr int GRID_Z = Chunk::SIZE_Z + 2 * MARGIN + 1;

	Colunm grid[GRID_X][GRID_Z];

	Colunm& at(int localX, int localZ) 
	{ 
		return grid[localX + MARGIN][localZ + MARGIN]; 
	}

	const Colunm& at(int localX, int localZ) const 
	{ 
		return grid[localX + MARGIN][localZ + MARGIN]; 
	}

	Colunm& at(ChunkCoord position)
	{ 
		return at(position.x, position.z); 
	}

	const Colunm& at(ChunkCoord position) const 
	{
		return at(position.x, position.z); 
	}
};