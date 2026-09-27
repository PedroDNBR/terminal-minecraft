#pragma once
#include "Rect.h"
struct InventoryLayout {
	int top, bottom;
	int startX;
	int slotWidth;
	int slotCount;
	int maxSlotsInRow;

    Rect slotRect(int slot)
    {
        int col = slot % maxSlotsInRow;
        int row = slot / maxSlotsInRow;

        int x0 = startX + (col * slotWidth);
        int y0 = top + (row * slotWidth);

        return {
            x0,
            y0,
            x0 + slotWidth,
            y0 + slotWidth
        };
    }
};