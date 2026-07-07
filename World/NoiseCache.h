#pragma once
#include "NoiseConstants.h"

struct NoiseCache
{
	float warpX[NoiseConstants::NOISE_GRID][NoiseConstants::NOISE_GRID];
	float warpZ[NoiseConstants::NOISE_GRID][NoiseConstants::NOISE_GRID];

	float temperature[NoiseConstants::NOISE_GRID][NoiseConstants::NOISE_GRID];
	float humidity[NoiseConstants::NOISE_GRID][NoiseConstants::NOISE_GRID];

	float base[NoiseConstants::NOISE_GRID][NoiseConstants::NOISE_GRID];
	float hills[NoiseConstants::NOISE_GRID][NoiseConstants::NOISE_GRID];
	float peaks[NoiseConstants::NOISE_GRID][NoiseConstants::NOISE_GRID];
	float valleys[NoiseConstants::NOISE_GRID][NoiseConstants::NOISE_GRID];

	float forest[NoiseConstants::NOISE_GRID][NoiseConstants::NOISE_GRID];

	float riverX[NoiseConstants::NOISE_GRID][NoiseConstants::NOISE_GRID];
	float riverZ[NoiseConstants::NOISE_GRID][NoiseConstants::NOISE_GRID];
};