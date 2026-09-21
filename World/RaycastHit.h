#include "../Core/Vector.h"
#include "Chunk/Block.h"
#pragma once
struct RaycastHit
{
	bool hit = false;
	Vector3Int block;
	Vector3Int placement;
	BlockType blockType = B_AIR;
};