#pragma once
#include "../../World/Chunk/Block.h"

struct InventorySlot {
	BlockType type = B_AIR;
	uint8_t count = 0;
	bool isEmpty() const { return count == 0; }
};