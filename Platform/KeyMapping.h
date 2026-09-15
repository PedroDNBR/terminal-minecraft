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
	ENTER,
	SPACE,
	Q,
	I,
	J,
	K,
	L,
	MAX
};

constexpr int KeyCount =
static_cast<int>(Key::MAX);