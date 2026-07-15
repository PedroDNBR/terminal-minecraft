#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include "Colors.h"
#include "ITerminalOutput.h"

class VTOutput : public ITerminalOutput
{
public:
	void present(const std::vector<uint8_t>& colorBuffer, const std::vector<RawTextPrint>& textToPrint, int realWidth, int realHeight, int logicalWidth, int logicalHeight);

private:
    void appendByte(std::string& out, uint8_t byte);

    void appendForeground(std::string& out, uint32_t rgbColor);
    void appendBackground(std::string& out, uint32_t rgbColor);

    std::string outputFrame;

    static constexpr uint32_t PALETTE[COLOR_MAX] = {
        /*[BLACK] = */              0x000000,
        /*[BLUE] = */               0x155cdf,
        /*[GREEN] = */              0x21a810,
        /*[CYAN] = */               0x0dabb1,
        /*[RED] = */                0x740c0c,
        /*[PURPLE] = */             0x870c92,
        /*[YELLOW] = */             0xaf760c,
        /*[WHITE] = */              0xbbbbbb,
        /*[GRAY] = */               0x757575,
        /*[BRIGHT_BLUE] = */        0x0099ff,
        /*[BRIGHT_GREEN] = */       0x1cf115,
        /*[BRIGHT_CYAN] = */        0x08f3e7,
        /*[BRIGHT_RED] = */         0xff0000,
        /*[BRIGHT_PURPLE] = */      0xec0ce1,
        /*[BRIGHT_YELLOW] = */      0xf8e806,
        /*[BRIGHT_WHITE] = */       0xffffff
    };
};

