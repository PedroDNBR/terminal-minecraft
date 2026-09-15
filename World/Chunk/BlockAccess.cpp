#include "BlockAccess.h"
#include "../../Terminal/Colors.h"

namespace
{
	Chunk* resolveChunk(Chunk* chunk, const BlockAccess::Neighbours& neighbours,
		Vector3Int position, int& localX, int& localZ)
	{
		localX = position.x;
		localZ = position.z;

		if (position.x < 0)
		{
			localX = position.x + Chunk::SIZE_X;
			return neighbours.negativeX;
		}
		if (position.x >= Chunk::SIZE_X)
		{
			localX = position.x - Chunk::SIZE_X;
			return neighbours.positiveX;
		}
		if (position.z < 0)
		{
			localZ = position.z + Chunk::SIZE_Z;
			return neighbours.negativeZ;
		}
		if (position.z >= Chunk::SIZE_Z)
		{
			localZ = position.z - Chunk::SIZE_Z;
			return neighbours.positiveZ;
		}

		return chunk;
	}
}

bool BlockAccess::isAir(Chunk* chunk, const Neighbours& neighbours, Vector3Int position)
{
	if (position.y < 0 || position.y >= Chunk::SIZE_Y)
		return true;

	int localX = 0, localZ = 0;
	Chunk* target = resolveChunk(chunk, neighbours, position, localX, localZ);
	if (target == nullptr)
		return true;

	return target->blocks[localX][localZ][position.y] == BlockType::B_AIR;
}

bool BlockAccess::isWater(Chunk* chunk, const Neighbours& neighbours, Vector3Int position)
{
	if (position.y < 0 || position.y >= Chunk::SIZE_Y)
		return true;

	int localX = 0, localZ = 0;
	Chunk* target = resolveChunk(chunk, neighbours, position, localX, localZ);
	if (target == nullptr)
		return true;

	return target->blocks[localX][localZ][position.y] == BlockType::B_WATER;
}

bool BlockAccess::isTransparent(Chunk* chunk, const Neighbours& neighbours, Vector3Int position)
{
	if (position.y < 0 || position.y >= Chunk::SIZE_Y)
		return true;

	int localX = 0, localZ = 0;
	Chunk* target = resolveChunk(chunk, neighbours, position, localX, localZ);
	if (target == nullptr)
		return true;

	BlockType blockType = target->blocks[localX][localZ][position.y];
	return blockType == BlockType::B_AIR || blockType == BlockType::B_WATER;
}

uint8_t BlockAccess::getSkyLight(Chunk* chunk, const Neighbours& neighbours, Vector3Int position)
{
	if (position.y < 0)
		return 0;
	if (position.y >= Chunk::SIZE_Y)
		return SHADE_LEVELS - 1;

	int localX = 0, localZ = 0;
	Chunk* target = resolveChunk(chunk, neighbours, position, localX, localZ);

	if (target == nullptr)
	{
		int clampedX = localX < 0 ? 0 : (localX >= Chunk::SIZE_X ? Chunk::SIZE_X - 1 : localX);
		int clampedZ = localZ < 0 ? 0 : (localZ >= Chunk::SIZE_Z ? Chunk::SIZE_Z - 1 : localZ);

		if (position.x < 0)                    clampedX = 0;
		else if (position.x >= Chunk::SIZE_X)  clampedX = Chunk::SIZE_X - 1;
		if (position.z < 0)                    clampedZ = 0;
		else if (position.z >= Chunk::SIZE_Z)  clampedZ = Chunk::SIZE_Z - 1;

		return chunk->skyLight[clampedX][clampedZ][position.y];
	}

	return target->skyLight[localX][localZ][position.y];
}
