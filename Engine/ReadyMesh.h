#pragma once
#include <vector>
#include "../World/Chunk/ChunkCoord.h"
#include "Quad.h"

struct ReadyMesh
{
	ChunkCoord coord;
	std::vector<Quad> quads;
};