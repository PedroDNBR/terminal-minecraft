#pragma once

enum class Key
{
	W,
	A,
	S,
	D,
	P,
	B,
	N,
	ESCAPE,
	LSHIFT,
	LCNTRL,
	ENTER,
	SPACE,
	Q,
	I,
	J,
	K,
	L,
	G,
	NUM_LINE_1,
	NUM_LINE_2,
	NUM_LINE_3,
	NUM_LINE_4,
	NUM_LINE_5,
	NUM_LINE_6,
	NUM_LINE_7,
	NUM_LINE_8,
	NUM_LINE_9,
	MAX
};

constexpr int KeyCount =
static_cast<int>(Key::MAX);