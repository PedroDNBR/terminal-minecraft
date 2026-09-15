#pragma once
#include <cstdint>
#include <unordered_map>
#include <shared_mutex>
#include "Chunk.h"
#include "ChunkCoord.h"
#include "ChunkModifications.h"

class WorldEdits
{
public:
	void set(ChunkCoord coord, uint16_t blockIndex, BlockType blockType)
	{
		std::unique_lock<std::shared_mutex> wlock(mutex);
		edits[coord].modifiedBlocks[blockIndex] = blockType;
	}

	ChunkModifications copyFor(ChunkCoord coord) const
	{
		std::shared_lock<std::shared_mutex> rlock(mutex);

		auto it = edits.find(coord);
		if (it == edits.end())
			return ChunkModifications{};

		return it->second;
	}

	bool empty() const
	{
		std::shared_lock<std::shared_mutex> rlock(mutex);
		return edits.empty();
	}

	static uint16_t packIndex(int localX, int localY, int localZ)
	{
		return (uint16_t)((localX * Chunk::SIZE_Z + localZ) * Chunk::SIZE_Y + localY);
	}

	static void unpackIndex(uint16_t blockIndex, int& localX, int& localY, int& localZ)
	{
		localY = blockIndex % Chunk::SIZE_Y;

		int xzIndex = blockIndex / Chunk::SIZE_Y;
		localZ = xzIndex % Chunk::SIZE_Z;
		localX = xzIndex / Chunk::SIZE_Z;
	}

private:
	std::unordered_map<ChunkCoord, ChunkModifications, ChunkCoordHash> edits;
	mutable std::shared_mutex mutex;
};
