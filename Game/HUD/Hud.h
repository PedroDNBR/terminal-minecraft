#pragma once
#include "../../Terminal/Renderer.h"
#include "../Entities/Player.h"
#include "HotbarLayout.h"
#include "InventoryLayout.h"

class Hud
{

public:
	void drawCrosshair(Renderer& renderer);

	void drawHotbar(const Player& player, Renderer& renderer);

	void drawInventory(const Player& player, Renderer& renderer);


private:
	HotbarLayout computeHotbarLayout(const Renderer& renderer, int totalSlots) const;
	InventoryLayout computeInventoryLayout(const Renderer& renderer, int columns, int rows) const;

	void drawSlotFill(Renderer& renderer, const Rect& rect, const InventorySlot& slot);
	void drawSlotBorder(Renderer& renderer, const Rect& rect, bool selected);
	void drawSlotText(Renderer& renderer, const Rect& rect, const InventorySlot& slot, bool selected);
};

