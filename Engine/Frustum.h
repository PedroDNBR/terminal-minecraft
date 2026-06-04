#pragma once
#include "Plane.h"

struct Frustum
{
	Plane planes[6];

	bool isBoxOutside(float minX, float minY, float minZ, float maxX, float maxY, float maxZ)
	{
		for(int i = 0; i < 6; i++)
		{
			const Plane& plane = planes[i];
			
			float planeX = (plane.a > 0) ? maxX : minX;
			float planeY = (plane.b > 0) ? maxY : minY;
			float planeZ = (plane.c > 0) ? maxZ : minZ;

			if (plane.distanceTo({planeX, planeY, planeZ}) < 0)
				return true;
		}
		return false;
	}
};