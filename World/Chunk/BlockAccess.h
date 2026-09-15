#pragma once
#include <cstdint>
#include "Chunk.h"
#include "Block.h"
#include "../../Core/Vector.h"

namespace BlockAccess
{
	struct Neighbours
	{
		Chunk* negativeX = nullptr;
		Chunk* positiveX = nullptr;
		Chunk* negativeZ = nullptr;
		Chunk* positiveZ = nullptr;
	};

	bool isAir(Chunk* chunk, const Neighbours& neighbours, Vector3Int position);
	bool isWater(Chunk* chunk, const Neighbours& neighbours, Vector3Int position);
	bool isTransparent(Chunk* chunk, const Neighbours& neighbours, Vector3Int position);

	uint8_t getSkyLight(Chunk* chunk, const Neighbours& neighbours, Vector3Int position);
}
