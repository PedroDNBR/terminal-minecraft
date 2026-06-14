#pragma once
#include "ChunkCoord.h"
struct ChunkLoadRequest
{
	ChunkCoord coord;
	float distanceSq;

	bool operator>(const ChunkLoadRequest& other) const {
		return distanceSq > other.distanceSq;
	}
};