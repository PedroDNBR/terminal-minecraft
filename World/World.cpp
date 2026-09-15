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
