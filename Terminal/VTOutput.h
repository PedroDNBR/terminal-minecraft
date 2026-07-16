#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <array>
#include "Colors.h"
#include "ITerminalOutput.h"

namespace VTPalette
{
    inline constexpr uint32_t BASE_PALETTE[COLOR_MAX] = {
        /*BLACK*/        0x000000,
        /*WHITE*/        0xFFFFFF,
        /*GRASS*/        0x73B349,
        /*DIRT*/         0x79553A,
        /*LOG*/          0x463926,
        /*LEAVES*/       0x408F2F,
        /*PLANK*/        0xB4905A,
        /*CACTUS*/       0x11831F,
        /*SAND*/         0xDAD49C,
        /*STONE*/        0x7F7F7F,
        /*COBBLESTONE*/  0x626262,
        /*WATER*/        0x002AFF,
        /*BEDROCK*/      0x333333,
        /*SKY*/          0x74BAF2
    };
    static_assert(sizeof(BASE_PALETTE) / sizeof(BASE_PALETTE[0]) == COLOR_MAX,
        "BASE e enum Color fora de sincronia");

    constexpr uint32_t shadeRGB(uint32_t rgb, int shade)
    {
        int percent = 20 + (80 * shade) / (SHADE_LEVELS - 1);
        int r = (int)((rgb >> 16) & 0xFF) * percent / 100;
        int g = (int)((rgb >> 8) & 0xFF) * percent / 100;
        int b = (int)(rgb & 0xFF) * percent / 100;
        return (uint32_t)((r << 16) | (g << 8) | b);
    }

    constexpr std::array<uint32_t, MAX_COLOR_SHADED> makeShaded()
    {
        std::array<uint32_t, MAX_COLOR_SHADED> p{};
        for (int color = 0; color < COLOR_MAX; color++)
            for (int shade = 0; shade < SHADE_LEVELS; shade++)
                p[color * SHADE_LEVELS + shade] = shadeRGB(BASE_PALETTE[color], shade);
        return p;
    }

    inline constexpr auto SHADED_PALETTE = makeShaded();
}

class VTOutput : public ITerminalOutput
{
public:
    void present(const std::vector<uint8_t>& colorBuffer,
        const std::vector<RawTextPrint>& textToPrint,
        int logicalWidth, int logicalHeight,
        int realWidth, int realHeight) override;
private:
    void appendByte(std::string& out, uint8_t byte);
    void appendForeground(std::string& out, uint32_t rgbColor);
    void appendBackground(std::string& out, uint32_t rgbColor);

    std::string outputFrame;
};