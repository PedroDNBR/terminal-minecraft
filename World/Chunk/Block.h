#pragma once
enum BlockType
{
	AIR = 0,
	GRASS,
	DIRT,
	STONE,
	LOG,
	LEAVES,
	WATER,
	SAND,
	CACTUS,
	BED_ROCK,
	BLOCK_MAX
};

struct BlockProperties
{
	int faceColors[6];
};