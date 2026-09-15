#pragma once
#include <array>
#include "../Platform/KeyMapping.h"
#include "InputActions.h"
#include <unordered_map>

class InputManager
{
public:
	void tick(float deltaTime);
	void bind(InputAction action, Key key);

	bool isActionDown(InputAction action);
	bool isActionPressed(InputAction action);
	bool isActionReleased(InputAction action);

	bool isKeyDown(Key key);
	bool isKeyPressed(Key key);
	bool isKeyReleased(Key key);

private:
	std::array<bool, KeyCount> current{};
	std::array<bool, KeyCount> previous{};

	std::array<std::vector<Key>, (int)InputAction::MAX> bindings;
};

