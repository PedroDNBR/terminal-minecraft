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
	uint8_t menuSelectedColor = colorIndex(C_WHITE, SHADE_LEVELS - 2);
	uint8_t menuColor = colorIndex(C_STONE, SHADE_LEVELS - 2);
	uint8_t menuBackColor = colorIndex(C_COBBLESTONE, SHADE_LEVELS - 2);

	int screenWidth = renderer.getLogicalWidth() - 1;
	int screenHeight = renderer.getLogicalHeight() - 1;

    int screenRealHeight = renderer.getRealHeight() - 1;

	int center = screenWidth / 2;
	
	int topHotbarBorder = (int)std::floorf((screenHeight * .9f));
	int hotbarHeight = screenHeight - topHotbarBorder;

	int slotWidth = hotbarHeight;
	int totalSlots = player.getHotbar().size();

	int totalHotbarWidth = totalSlots * slotWidth;

	int startX = center - (totalHotbarWidth / 2);

	int selectedSlot = player.getCurrentHotbarSlotSelected();

    for (int slot = 0; slot < totalSlots; slot++)
    {
        int x0 = startX + (slot * slotWidth);
        int x1 = x0 + slotWidth;

        BlockType currentBlock = player.getHotbar()[slot].type;
        uint8_t currentBlockCount = player.getHotbar()[slot].count;

        for (int y = topHotbarBorder + 1; y < screenHeight; y++)
        {
            for (int x = x0 + 1; x < x1; x++)
            {
                if (currentBlockCount > 0)
                    renderer.drawPixel(x, y, colorIndex(
                        static_cast<Color>(Blocks::PROPERTIES[currentBlock].faceColors[4]), SHADE_LEVELS - 1)
                    );
                else
                    renderer.drawPixel(x, y, menuBackColor);
            }
        }
        if (currentBlockCount > 0)
        {
            int blockCount = player.getHotbar()[slot].count;
            if(slot == selectedSlot)
            {
                renderer.queueText(
                    x1 - Blocks::BlockTypeNames[currentBlock].length() - std::to_string(blockCount).length() - 2, screenRealHeight,
                    std::string(Blocks::BlockTypeNames[currentBlock]) + " x" + std::to_string(blockCount),
                    C_WHITE
                );
            }
            else
            {
                renderer.queueText(
                    x1 - std::to_string(blockCount).length(), screenRealHeight,
                    std::to_string(blockCount),
                    C_WHITE
                );
			}
        }
    }

    for (int slot = 0; slot < totalSlots; slot++)
    {
        int x0 = startX + (slot * slotWidth);
        int x1 = x0 + slotWidth;

        uint8_t borderColor =
            (slot == selectedSlot)
            ? menuSelectedColor
            : menuColor;

        for (int x = x0; x <= x1; x++)
        {
            renderer.drawPixel(x, topHotbarBorder, borderColor);
            renderer.drawPixel(x, screenHeight, borderColor);
        }
    }

    for (int slot = 0; slot <= totalSlots; slot++)
    {
        int x = startX + (slot * slotWidth);

        bool selectedBorder =
            (slot == selectedSlot) ||
            (slot == selectedSlot + 1);

        uint8_t borderColor =
            selectedBorder
            ? menuSelectedColor
            : menuColor;

        for (int y = topHotbarBorder; y <= screenHeight; y++)
        {
            renderer.drawPixel(x, y, borderColor);
        }
    }
}
