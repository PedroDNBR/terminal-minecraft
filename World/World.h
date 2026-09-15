#pragma once
#include "ChunkManager.h"
#include "Chunk/Block.h"
#include "../Core/Vector.h"

class World
{
public:
	explicit World(ChunkManager& chunkManager) : chunkManager(chunkManager) {}

	BlockType getBlockAtWorldPosition(Vector3Int worldPosition) const;

	void setBlockAtWorldPosition(Vector3Int worldPosition, BlockType blockType);

private:
	ChunkManager& chunkManager;
};
