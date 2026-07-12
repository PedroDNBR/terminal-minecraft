#pragma once
#include "../Core/Vector.h"
struct Plane
{
	float a, b, c, d;

	float distanceTo(Vector3 position) const
	{
		return a * position.x + b * position.y + c * position.z + d;
	}
};