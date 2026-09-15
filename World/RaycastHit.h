#include "../Core/Vector.h"
#pragma once
struct RaycastHit
{
	bool hit = false;
	Vector3Int block;
	Vector3Int placement;
};