#pragma once
#include <vector>
#include "../World/Chunk/ChunkCoord.h"
#include "../Renderer/Quad.h"

struct ReadyMesh
{
	ChunkCoord coord;
	std::vector<Quad> quads;
};