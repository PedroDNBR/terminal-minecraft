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
	MAX
};

constexpr int KeyCount =
static_cast<int>(Key::MAX);