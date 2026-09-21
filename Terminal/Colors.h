#pragma once
enum Color {
    C_BLACK,
    C_BEDROCK,
    C_SKY_NIGHT,
    C_COBBLESTONE,
    C_DIRT,
    C_LOG,
	C_LEAVES,
    C_PLANK,
    C_CACTUS,
    C_WATER,
    C_STONE,
    C_SAND,
    C_GRASS,
    C_SKY,
    C_WHITE,
	COLOR_MAX
};
    
constexpr uint8_t SHADE_LEVELS = 8;
constexpr uint8_t MAX_COLOR_SHADED = COLOR_MAX * SHADE_LEVELS;

constexpr uint8_t colorIndex(Color color, int shade)
{
    if (shade < 0)
        shade = 0;
    if (shade >= SHADE_LEVELS) shade = SHADE_LEVELS - 1;

    return (uint8_t)(color * SHADE_LEVELS + shade);
}