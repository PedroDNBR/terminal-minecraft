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
	BED_ROCK,
	MAX
};

struct BlockProperties
{
	int faceColors[6];
};