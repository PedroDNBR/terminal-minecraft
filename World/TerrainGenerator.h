#pragma once
#include <vector>
#include <memory>
#include <unordered_map>
#include "Noise/NoiseConstants.h"
#include "Noise/NoiseCache.h"
#include "Chunk/Chunk.h"
#include "../Engine/Quad.h"
#include "Chunk/Colunm.h"
class ChunkManager;

class TerrainGenerator
{
public:
	std::unique_ptr<Chunk> generateChunkData(ChunkCoord chunkPosition, ChunkManager& chunkManager);

	NoiseCache buildNoiseCache(ChunkCoord chunkPosition, uint32_t seed);

	Colunm generateColunmData(ChunkCoord localPosition, const NoiseCache& noiseCache);

	BlockType surfaceBlockType(int y, const Colunm& colunm);

	static BlockType blockAt(Chunk* chunk, int localX, int localY, int localZ);

	static void stampBlock(Chunk* chunk, int localX, int localY, int localZ, BlockType blockType);
	static unsigned featureOriginHash(int worldX, int worldZ, unsigned seed);
	static unsigned featureShapeHash(int worldX, int worldZ, int baseY);
	static unsigned featureWormHash(int worldX, int worldZ, unsigned seed);

	void placeTree(Chunk* chunk, int localX, int baseY, int localZ, int worldX, int worldZ);
	void placeCactus(Chunk* chunk, int localX, int baseY, int localZ, int worldX, int worldZ);

	void carveWormCave(
		Chunk* chunk,
		int worldOffsetX,
		int worldOffsetZ,
		float startWorldX,
		float startWorldZ,
		float startY,
		float directionX,
		float directionY,
		float directionZ,
		float radius,
		int length
	);

	float bilinearLerp(const float grid[NoiseConstants::NOISE_GRID_PADDED][NoiseConstants::NOISE_GRID_PADDED],
		int intX, int intZ, float fractionX, float fractionZ);

	bool isDryBiome(const Colunm& colunm);

	int seed = 421894;
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

	static constexpr float DESERT_FLATTEN = .8f;
	static constexpr float BIOME_ELEVATION = 14.0f;

	static constexpr float OFFSET_ROUND = 2.f;

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

	static constexpr float MIN_CACTUS_DENSITY = 0.7f;
	static constexpr int CACTUS_PLACEMENT_OFFSET = 350;
	static constexpr int CACTUS_TRUNK_BASE_HEIGHT = 3;

	static constexpr int ROCK_MIN_HEIGHT = 58;
	static constexpr int ROCK_SLOPE_MIN_HEIGHT = 50;
	static constexpr float ROCKY_MIN_STEEPNESS = 0.6f;
};

