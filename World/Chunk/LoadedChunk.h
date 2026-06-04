#pragma once
struct LoadedChunk
{
	ChunkCoord coord;
	std::unique_ptr<Chunk> chunk;
};