#pragma once
#include <vector>
#include "../World/Chunk/ChunkCoord.h"
#include "../Renderer/Quad.h"

struct VisibleMesh
{
	ChunkCoord coord;
	const std::vector<Quad>* quads;
};