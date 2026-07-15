#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <wtypes.h>
#include "Colors.h"
#include "ITerminalOutput.h"

class WinWriteConsoleOutput : public ITerminalOutput
{
public:
    void present(const std::vector<uint8_t>& colorBuffer, const std::vector<RawTextPrint>& textToPrint, int realWidth, int realHeight, int logicalWidth, int logicalHeight);

private:
    HANDLE handle;
    DWORD starterMode;
    SMALL_RECT rect;

    std::vector<CHAR_INFO> screenBuffer;

    static constexpr WORD PALETTE[COLOR_MAX] = {
        /*[BLACK] = */              0,
        /*[BLUE] = */               1,
        /*[GREEN] = */              2,
        /*[CYAN] = */               3,
        /*[RED] = */                4,
        /*[PURPLE] = */             5,
        /*[YELLOW] = */             6,
        /*[WHITE] = */              7,
        /*[GRAY] = */               8,
        /*[BRIGHT_BLUE] = */        9,
        /*[BRIGHT_GREEN] = */       10,
        /*[BRIGHT_CYAN] = */        11,
        /*[BRIGHT_RED] = */         12,
        /*[BRIGHT_PURPLE] = */      13,
        /*[BRIGHT_YELLOW] = */      14,
        /*[BRIGHT_WHITE] = */       15
    };

    void drawText(int x, int y, const std::string& text, uint8_t color, int realWidth, int realHeight, int logicalWidth, int logicalHeight);
};
