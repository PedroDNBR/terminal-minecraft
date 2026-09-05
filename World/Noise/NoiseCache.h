#pragma once
#include "NoiseConstants.h"

struct NoiseCache
{
	float temperature[NoiseConstants::NOISE_GRID_PADDED][NoiseConstants::NOISE_GRID_PADDED];
	float humidity[NoiseConstants::NOISE_GRID_PADDED][NoiseConstants::NOISE_GRID_PADDED];

	float base[NoiseConstants::NOISE_GRID_PADDED][NoiseConstants::NOISE_GRID_PADDED];
	float hills[NoiseConstants::NOISE_GRID_PADDED][NoiseConstants::NOISE_GRID_PADDED];
	float peaks[NoiseConstants::NOISE_GRID_PADDED][NoiseConstants::NOISE_GRID_PADDED];
	float valleys[NoiseConstants::NOISE_GRID_PADDED][NoiseConstants::NOISE_GRID_PADDED];

	float forest[NoiseConstants::NOISE_GRID_PADDED][NoiseConstants::NOISE_GRID_PADDED];
};