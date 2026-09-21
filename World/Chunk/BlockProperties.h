#pragma once
#include <cstdint>
#include <string>
#include "Block.h"
#include "../../Terminal/Colors.h"

namespace Blocks
{
	enum Density : uint8_t
	{
		AIR,
		FLUID_THIN,
		FLUID_DENSE,
		SOLID,
		MAX
	};

	inline constexpr uint8_t DENSITY[BlockType::BLOCK_MAX] = {
		/* B_AIR      */ Density::AIR,
		/* B_GRASS    */ Density::SOLID,
		/* B_DIRT     */ Density::SOLID,
		/* B_STONE    */ Density::SOLID,
		/* B_LOG      */ Density::SOLID,
		/* B_LEAVES   */ Density::SOLID,
		/* B_WATER    */ Density::FLUID_THIN,
		/* B_SAND     */ Density::SOLID,
		/* B_CACTUS   */ Density::SOLID,
		/* B_BEDROCK  */ Density::SOLID
	};

	inline constexpr uint8_t OPAQUE_THRESHOLD = 7;

	inline constexpr uint8_t OPACITY[BlockType::BLOCK_MAX] = {
		/* B_AIR      */ 0,
		/* B_GRASS    */ OPAQUE_THRESHOLD,
		/* B_DIRT     */ OPAQUE_THRESHOLD,
		/* B_STONE    */ OPAQUE_THRESHOLD,
		/* B_LOG      */ OPAQUE_THRESHOLD,
		/* B_LEAVES   */ 2,       
		/* B_WATER    */ 1,
		/* B_SAND     */ OPAQUE_THRESHOLD,
		/* B_CACTUS   */ OPAQUE_THRESHOLD,
		/* B_BEDROCK  */ OPAQUE_THRESHOLD
	};

	inline constexpr BlockProperties PROPERTIES[BlockType::BLOCK_MAX] = {
		/* B_AIR     */ { Color::C_BLACK,  Color::C_BLACK,  Color::C_BLACK,  Color::C_BLACK,  Color::C_BLACK,  Color::C_BLACK },
		/* B_GRASS   */ { Color::C_DIRT,   Color::C_DIRT,   Color::C_DIRT,   Color::C_DIRT,   Color::C_GRASS,  Color::C_DIRT },
		/* B_DIRT    */ { Color::C_DIRT,   Color::C_DIRT,   Color::C_DIRT,   Color::C_DIRT,   Color::C_DIRT,   Color::C_DIRT },
		/* B_STONE   */ { Color::C_STONE,  Color::C_STONE,  Color::C_STONE,  Color::C_STONE,  Color::C_STONE,  Color::C_STONE },
		/* B_LOG     */ { Color::C_LOG,    Color::C_LOG,    Color::C_LOG,    Color::C_LOG,    Color::C_LOG,    Color::C_LOG },
		/* B_LEAVES  */ { Color::C_LEAVES, Color::C_LEAVES, Color::C_LEAVES, Color::C_LEAVES, Color::C_LEAVES, Color::C_LEAVES },
		/* B_WATER   */ { Color::C_WATER,  Color::C_WATER,  Color::C_WATER,  Color::C_WATER,  Color::C_WATER,  Color::C_WATER },
		/* B_SAND    */ { Color::C_SAND,   Color::C_SAND,   Color::C_SAND,   Color::C_SAND,   Color::C_SAND,   Color::C_SAND },
		/* B_CACTUS  */ { Color::C_CACTUS, Color::C_CACTUS, Color::C_CACTUS, Color::C_CACTUS, Color::C_CACTUS, Color::C_CACTUS },
		/* B_BEDROCK */ { Color::C_BEDROCK,Color::C_BEDROCK,Color::C_BEDROCK,Color::C_BEDROCK,Color::C_BEDROCK,Color::C_BEDROCK }
	};

	inline constexpr std::string_view BlockTypeNames[BlockType::BLOCK_MAX] = {
		/* B_AIR     */ "Air",
		/* B_GRASS   */ "Grass",
		/* B_DIRT    */ "Dirt",
		/* B_STONE   */ "Stone",
		/* B_LOG     */ "Log",
		/* B_LEAVES  */ "Leaves",
		/* B_WATER   */ "Water",
		/* B_SAND    */ "Sand",
		/* B_CACTUS  */ "Cactus",
		/* B_BEDROCK */ "Bedrock"
	};

	static_assert(sizeof(DENSITY) / sizeof(DENSITY[0]) == BlockType::BLOCK_MAX,
		"DENSITY out of sync with BlockType enum");
	static_assert(sizeof(OPACITY) / sizeof(OPACITY[0]) == BlockType::BLOCK_MAX,
		"OPACITY out of sync with BlockType enum");
	static_assert(sizeof(PROPERTIES) / sizeof(PROPERTIES[0]) == BlockType::BLOCK_MAX,
		"PROPERTIES out of sync with BlockType enum");
	static_assert(sizeof(BlockTypeNames) / sizeof(BlockTypeNames[0]) == BlockType::BLOCK_MAX,
		"BlockTypeNames out of sync with BlockType enum");
}
