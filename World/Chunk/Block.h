#pragma once
enum BlockType
{
	B_AIR = 0,
	B_GRASS,
	B_DIRT,
	B_STONE,
	B_LOG,
	B_LEAVES,
	B_WATER,
	B_SAND,
	B_CACTUS,
	B_BEDROCK,
	BLOCK_MAX
};

struct BlockProperties
{
	int faceColors[6];
};