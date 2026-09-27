#pragma once
#include "Rect.h"

struct HotbarLayout {
	int top, bottom;
	int startX;
	int slotWidth;
	int slotCount;

	Rect slotRect(int slot) const
	{
		int x0 = startX + (slot * slotWidth);
		return {
			x0,
			top,
			x0 + slotWidth,
			bottom
		};
	}
};