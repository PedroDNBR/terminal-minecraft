#include "InputManager.h"
#include "../Platform/Platform.h"

void InputManager::tick(float deltaTime)
{
	previous = current;
	for (size_t i = 0; i < KeyCount; i++)
	{
		Key key = static_cast<Key>(i);
		current[i] = Platform::isKeyDown(key);
	}
}

void InputManager::bind(InputAction action, Key key)
{
	bindings[(int)action].push_back(key);
}

bool InputManager::isActionDown(InputAction action)
{
	const auto& keys = bindings[(int)action];
	for (Key key : keys)
		if (current[(int)key]) return true;

	return false;
}

bool InputManager::isActionPressed(InputAction action)
{
	const auto& keys = bindings[(int)action];
	for (Key key : keys)
		if (current[(int)key] && !previous[(int)key])
			return true;

	return false;
}

bool InputManager::isActionReleased(InputAction action)
{
	const auto& keys = bindings[(int)action];
	for (Key key : keys)
		if (!current[(int)key] && previous[(int)key])
			return true;

	return false;
}

bool InputManager::isKeyDown(Key key)
{
	return current[static_cast<int>(key)];
}

bool InputManager::isKeyPressed(Key key)
{
	return current[static_cast<int>(key)] && !previous[static_cast<int>(key)];
}

bool InputManager::isKeyReleased(Key key)
{
	return !current[static_cast<int>(key)] && previous[static_cast<int>(key)];
}