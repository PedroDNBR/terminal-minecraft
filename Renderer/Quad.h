#include "Vector.h"

#pragma once
struct Quad
{
	/*Vector3 v0, v1, v2, v3;
	Vector3Int normal;
	int color;*/

	uint8_t faceIndex;
	uint8_t normal;
	uint8_t uStart;
	uint8_t vStart;
	uint8_t uSpan;
	uint8_t vSpan;
	int color;
};