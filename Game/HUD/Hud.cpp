#include "Hud.h"
#include "../../World/Chunk/BlockProperties.h"

void Hud::drawCrosshair(Renderer& renderer)
{
	int crosshairX = renderer.getLogicalWidth() / 2;
	int crosshairY = renderer.getLogicalHeight() / 2;

	uint8_t crosshairColor = colorIndex(C_WHITE, SHADE_LEVELS - 1);

	if(renderer.getPixelColor(crosshairX, crosshairY) > 9)
		crosshairColor = colorIndex(C_BLACK, SHADE_LEVELS - 1);

	renderer.drawPixel(crosshairX, crosshairY, crosshairColor);
	renderer.drawPixel(crosshairX - 1, crosshairY, crosshairColor);
	renderer.drawPixel(crosshairX + 1, crosshairY, crosshairColor);
	renderer.drawPixel(crosshairX, crosshairY - 1, crosshairColor);
	renderer.drawPixel(crosshairX, crosshairY + 1, crosshairColor);
}

void Hud::drawHotbar(const Player& player, Renderer& renderer)
{
	const auto& hotbar = player.getHotbar();
    HotbarLayout layout = computeHotbarLayout(renderer, hotbar.size());
    int selected = player.getCurrentHotbarSlotSelected();

    for (int slot = 0; slot < layout.slotCount; slot++)
        drawSlotFill(renderer, layout.slotRect(slot), hotbar[slot]);

    for (int slot = 0; slot < layout.slotCount; slot++)
        if(slot != selected)
            drawSlotBorder(renderer, layout.slotRect(slot), false);

    drawSlotBorder(renderer, layout.slotRect(selected), true);

    for (int slot = 0; slot < layout.slotCount; slot++)
    {
        drawSlotText(renderer, layout.slotRect(slot), hotbar[slot], slot == selected);
    }
}

HotbarLayout Hud::computeHotbarLayout(const Renderer& renderer, int totalSlots) const
{
    const int screenWidth = renderer.getLogicalWidth() - 1;
    const int screenHeight = renderer.getLogicalHeight() - 1;

    const int screenRealHeight = renderer.getRealHeight() - 1;

    int center = screenWidth / 2;

    int topHotbarBorder = (int)std::floorf((screenHeight * .9f));
    int hotbarHeight = screenHeight - topHotbarBorder;

    int slotWidth = hotbarHeight;

    return {
        topHotbarBorder,
        screenHeight,
        center - (totalSlots * slotWidth / 2),
        slotWidth,
		totalSlots
    };
}

void Hud::drawSlotFill(Renderer& renderer, const Rect& rect, const InventorySlot& slot)
{
    uint8_t color = !slot.isEmpty()
        ? colorIndex((Color)Blocks::PROPERTIES[slot.type].faceColors[4], SHADE_LEVELS - 1)
        : colorIndex(C_COBBLESTONE, SHADE_LEVELS - 2);

    renderer.drawFilledRect(rect.x0 + 1, rect.y0 + 1, rect.x1 - 1, rect.y1 - 1, color);
}

void Hud::drawSlotBorder(Renderer& renderer, const Rect& rect, bool selected)
{
    uint8_t color = 
        selected
        ? colorIndex(C_WHITE, SHADE_LEVELS - 2)
        : colorIndex(C_STONE, SHADE_LEVELS - 2);

    renderer.drawRectBorder(rect.x0, rect.y0, rect.x1, rect.y1, color);
}

void Hud::drawSlotText(Renderer& renderer, const Rect& rect, const InventorySlot& slot, bool selected)
{
    if (slot.isEmpty()) return;
    int screenRealHeight = renderer.getRealHeight() - 1;
    if (selected)
    {
        renderer.queueText(
            rect.x1 - Blocks::BlockTypeNames[slot.type].length() - std::to_string(slot.count).length() - 2, renderer.logicalToCellY(rect.y1),
            std::string(Blocks::BlockTypeNames[slot.type]) + " x" + std::to_string(slot.count),
            C_WHITE
        );
    }
    else
    {
        renderer.queueText(
            rect.x1 - std::to_string(slot.count).length(), renderer.logicalToCellY(rect.y1),
            std::to_string(slot.count),
            C_WHITE
        );
    }
}
