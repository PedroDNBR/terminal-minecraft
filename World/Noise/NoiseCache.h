#pragma once
#include "NoiseConstants.h"

struct NoiseCache
{
	float temperature[NoiseConstants::NOISE_GRID][NoiseConstants::NOISE_GRID];
	float humidity[NoiseConstants::NOISE_GRID][NoiseConstants::NOISE_GRID];

	float base[NoiseConstants::NOISE_GRID][NoiseConstants::NOISE_GRID];
	float hills[NoiseConstants::NOISE_GRID][NoiseConstants::NOISE_GRID];
	float peaks[NoiseConstants::NOISE_GRID][NoiseConstants::NOISE_GRID];
	float valleys[NoiseConstants::NOISE_GRID][NoiseConstants::NOISE_GRID];

	float forest[NoiseConstants::NOISE_GRID][NoiseConstants::NOISE_GRID];
};