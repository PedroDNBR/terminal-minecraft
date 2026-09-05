#pragma once
#include "../Chunk/Chunk.h"

namespace NoiseConstants
{
	inline constexpr int NOISE_STEP = 4;
	inline constexpr int NOISE_GRID = Chunk::SIZE_X / NOISE_STEP + 1;

	inline constexpr int NOISE_MARGIN = 1;
	inline constexpr int NOISE_GRID_PADDED = NOISE_GRID + 2 * NOISE_MARGIN;

	inline constexpr float WARP_FREQUENCY = 0.008f;

	inline constexpr float BIOME_FREQUENCY = 0.005f;

	inline constexpr float BASE_FREQUENCY = 0.02f;
	inline constexpr float HILLS_FREQUENCY = 0.05f;
	inline constexpr float PEAKS_FREQUENCY = 0.04f;
	inline constexpr float VALLEYS_FREQUENCY = 0.03f;
	inline constexpr float FOREST_FREQUENCY = 0.02f;

	inline constexpr int CAVE_STEP = 8;

	inline constexpr int CAVE_GRID_X = Chunk::SIZE_X / CAVE_STEP + 2;
	inline constexpr int CAVE_GRID_Y = Chunk::SIZE_Y / CAVE_STEP + 2;
	inline constexpr int CAVE_GRID_Z = Chunk::SIZE_Z / CAVE_STEP + 2;
}
