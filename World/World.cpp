#include "World.h"
#include "Chunk/WorldCoords.h"

BlockType World::getBlockAtWorldPosition(Vector3Int worldPosition) const
{
	if (!WorldCoords::isInsideWorldHeight(worldPosition.y))
		return BlockType::B_AIR;

	ChunkCoord coord = WorldCoords::toChunk(worldPosition);
	Vector3Int local = WorldCoords::toLocal(worldPosition);

	std::shared_lock<std::shared_mutex> rlock(chunkManager.chunksMutex);

	Chunk* chunk = chunkManager.getChunk(coord);
	if (chunk == nullptr)
		return BlockType::B_AIR;

	return chunk->blocks[local.x][local.z][local.y];
}

void World::setBlockAtWorldPosition(Vector3Int worldPosition, BlockType blockType)
{
	if (!WorldCoords::isInsideWorldHeight(worldPosition.y))
		return;

	ChunkCoord coord = WorldCoords::toChunk(worldPosition);
	Vector3Int local = WorldCoords::toLocal(worldPosition);

	chunkManager.edits.set(coord, WorldEdits::packIndex(local.x, local.y, local.z), blockType);
	chunkManager.requestChunkRebuild(coord);
}

RaycastHit World::raycast(Vector3 origin, Vector3 direction, float maxDistance) const
{
	Vector3Int block = { 
		(int)std::floorf(origin.x), 
		(int)std::floorf(origin.y),
		(int)std::floorf(origin.z) 
	};
	Vector3Int step = { 
		direction.x > 0 ? 1 : (direction.x < 0 ? -1 : 0),
		direction.y > 0 ? 1 : (direction.y < 0 ? -1 : 0),
		direction.z > 0 ? 1 : (direction.z < 0 ? -1 : 0)
	};
	Vector3 tDelta = {
		fabsf(1.0f / direction.x),
		fabsf(1.0f / direction.y),
		fabsf(1.0f / direction.z)
	};
	Vector3 tMax;
	tMax.x = (direction.x > 0 ? (block.x + 1 - origin.x) : (block.x - origin.x)) / direction.x;
	tMax.y = (direction.y > 0 ? (block.y + 1 - origin.y) : (block.y - origin.y)) / direction.y;
	tMax.z = (direction.z > 0 ? (block.z + 1 - origin.z) : (block.z - origin.z)) / direction.z;

	if (fabsf(direction.x) < 1e-8f) 
	{ 
		tDelta.x = 1e30f; 
		tMax.x = 1e30f; 
	}
	if (fabsf(direction.y) < 1e-8f) 
	{ 
		tDelta.y = 1e30f; 
		tMax.y = 1e30f; 
	}
	if (fabsf(direction.z) < 1e-8f) 
	{ 
		tDelta.z = 1e30f; 
		tMax.z = 1e30f; 
	}

	Vector3Int previousBlock = block;
	float t = 0.0f;
	while (t <= maxDistance)
	{
		previousBlock = block;
		if (tMax.x < tMax.y && tMax.x < tMax.z)
		{
			block.x += step.x;
			t = tMax.x;
			tMax.x += tDelta.x;
		}
		else if (tMax.y < tMax.z)
		{
			block.y += step.y;
			t = tMax.y;
			tMax.y += tDelta.y;
		}
		else
		{
			block.z += step.z;
			t = tMax.z;
			tMax.z += tDelta.z;
		}

		BlockType blockType = getBlockAtWorldPosition(block);
		if (blockType != BlockType::B_AIR && blockType != BlockType::B_WATER)
		{
			return {
				true,
				block,
				previousBlock,
				blockType
			};
		}
	}
	return {};
}
