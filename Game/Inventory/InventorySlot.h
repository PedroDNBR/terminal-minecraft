#pragma once
#include "../../World/Chunk/Block.h"

struct InventorySlot {
	BlockType type;
	uint8_t count = 0;
};