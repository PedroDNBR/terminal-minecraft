#pragma once
#include "Chunk.h"
#include "ChunkCoord.h"
#include "../../Core/Vector.h"

namespace WorldCoords
{
	static_assert(Chunk::SIZE_X == 16 && Chunk::SIZE_Z == 16,
		"the mask/shift shortcut assumes the chunk is 16x16; use floorDiv with you change the size of it");

	inline ChunkCoord toChunk(Vector3Int worldPosition)
	{
		return { worldPosition.x >> 4, worldPosition.z >> 4 };
	}

	inline Vector3Int toLocal(Vector3Int worldPosition)
	{
		return { worldPosition.x & 15, worldPosition.y, worldPosition.z & 15 };
	}

	inline bool isInsideWorldHeight(int worldY)
	{
		return worldY >= 0 && worldY < Chunk::SIZE_Y;
	}
}
