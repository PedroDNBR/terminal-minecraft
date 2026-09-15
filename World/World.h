#pragma once
#include "ChunkManager.h"
#include "Chunk/Block.h"
#include "RaycastHit.h"
#include "../Core/Vector.h"

class World
{
public:
	explicit World(ChunkManager& chunkManager) : chunkManager(chunkManager) {}

	BlockType getBlockAtWorldPosition(Vector3Int worldPosition) const;

	void setBlockAtWorldPosition(Vector3Int worldPosition, BlockType blockType);

	RaycastHit raycast(Vector3 origin, Vector3 direction, float maxDistance) const;

private:
	ChunkManager& chunkManager;
};
