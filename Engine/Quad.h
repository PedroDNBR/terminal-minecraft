#include "../Core/Vector.h"

#pragma once
struct Quad
{
	uint8_t faceIndex;
	uint8_t normal;
	uint8_t uStart;
	uint8_t vStart;
	uint8_t uSpan;
	uint8_t vSpan;
	uint8_t color;
	uint8_t skyLight;
};