#pragma once
#include <optional>
#include "../../World/Chunk/Block.h"

struct InventorySlot {
	std::optional<BlockType> type;
	uint8_t count = 0;
};