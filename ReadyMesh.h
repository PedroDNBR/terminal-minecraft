#pragma once
#include <vector>
#include "Renderer/Quad.h"
#include "World/Chunk/ChunkCoord.h"

struct ReadyMesh {
	ChunkCoord coord;
	std::vector<Quad> quads;
};