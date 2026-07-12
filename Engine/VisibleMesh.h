#pragma once
#include <vector>
#include "../World/Chunk/ChunkCoord.h"
#include "Quad.h"

struct VisibleMesh
{
	ChunkCoord coord;
	const std::vector<Quad>* quads;
};