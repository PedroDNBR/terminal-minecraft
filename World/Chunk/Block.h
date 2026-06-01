#pragma once
enum BlockType
{
	AIR = 0,
	GRASS,
	DIRT,
	STONE,
	WOOD,
	LEAVES,
	WATER,
	SAND,
	BED_ROCK,
	MAX
};

struct BlockProperties
{
	int faceColors[6];
};