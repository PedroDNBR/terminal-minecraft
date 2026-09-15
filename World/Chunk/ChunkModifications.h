#pragma once
#include <unordered_map>
#include "Block.h"
struct ChunkModifications
{
	std::unordered_map<uint16_t, BlockType> modifiedBlocks;
};