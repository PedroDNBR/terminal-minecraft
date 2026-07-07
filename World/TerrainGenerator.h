#pragma once
#include <vector>
#include <memory>
#include <unordered_map>
#include "NoiseConstants.h"
#include "NoiseCache.h"
#include "Chunk/Chunk.h"
#include "../Renderer/Quad.h"

class TerrainGenerator
{
public:
	std::unique_ptr<Chunk> generateChunkData(ChunkCoord chunkPosition);

	NoiseCache buildNoiseCache(ChunkCoord chunkPosition, uint32_t seed);

	void placeTree(Chunk* chunk, int x, int baseY, int z);
	void placeCactus(Chunk* chunk, int x, int baseY, int z);

	void carveWormCave(
		Chunk* chunk,
		int heightMap[Chunk::SIZE_X][Chunk::SIZE_Z],
		float startX, float startY, float startZ,
		float directionX, float directionY, float directionZ,
		float radius, int length
	);

	float bilinearLerp(float grid[NoiseConstants::NOISE_GRID][NoiseConstants::NOISE_GRID],
		int intX, int intZ, float fractionX, float fractionZ);

	int seed = 1;
	static constexpr int BASE_TERRAIN_HEIGHT = 26;

	static constexpr float PEAK_SHAPE_SCALE = 2.0f;
	static constexpr float PEAK_SHAPE_OFFSET = 1.0f;
	static constexpr float PEAK_SHAPE_MAX = 1.0f;

	static constexpr float EROSION_START = 0.3f;
	static constexpr float EROSION_END = 0.7f;
	static constexpr float EROSION_MASK_MAX = 1.0f;

	static constexpr float VALLEY_MIDPOINT = 0.5f;
	static constexpr float VALLEY_STRENGTH = 18.0f;
	static constexpr float NEGATIVE_VALLEY_MULTIPLIER = 1.4f;

	static constexpr float CONTINENT_ELEVATION_MAX = 10.f;
	static constexpr float EROSION_PEAKS_MAX = 6.f;

	static constexpr float VALLEY_SCALE = 1.8f;

	static constexpr float OFFSET_ROUND = 2.f;

	static constexpr float RIVER_WIDTH = .12f;
	static constexpr float RIVER_MAX_STEEP = .4f;
	
	static constexpr int SEA_LEVEL = 24;

	static constexpr float CAVE_THRESHOUD = .2f;

	static constexpr int WORM_CAVE_FREQUENCY = 3;

	static constexpr float WORM_CAVE_RADIUS = 3.2f;
	static constexpr int WORM_CAVE_LENGTH = 30;

	static constexpr float MIN_FOREST_DENSITY = 0.25f;
	static constexpr int FOREST_TREE_PLACEMENT_OFFSET = 150;
	static constexpr int TREE_TRUNK_BASE_HEIGHT = 4;
	static constexpr int TREE_TRUNK_LEAVES_XZ_RADIUS = 2;
	static constexpr int TREE_TRUNK_LEAVES_Y_RADIUS = 2;

	static constexpr int CACTUS_TRUNK_BASE_HEIGHT = 3;



};

