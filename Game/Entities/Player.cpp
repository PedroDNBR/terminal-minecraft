#include "Player.h"
#include <Windows.h>

void Player::setPosition(const Vector3& newPosition)
{
	position = newPosition;
	camera.position = position;
}

void Player::setRotation(const Vector3& newRotation)
{
	rotation = newRotation;
	camera.yaw = rotation.y * 3.14159f / 180;
	camera.pitch = rotation.x * 3.14159f / 180;
}

void Player::tick(float deltaTime)
{
	if (GetKeyState('W') & 0x8000) setPosition(getPosition() + getCamera().getForward() * movementSpeed * deltaTime);
	if (GetKeyState('S') & 0x8000) setPosition(getPosition() + getCamera().getForward() * -movementSpeed * deltaTime);
	if (GetKeyState('D') & 0x8000) setPosition(getPosition() + getCamera().getRight() * movementSpeed * deltaTime);
	if (GetKeyState('A') & 0x8000) setPosition(getPosition() + getCamera().getRight() * -movementSpeed * deltaTime);

	if (GetKeyState('J') & 0x8000) setRotation(getRotation() + Vector3{ 0, cameraSpeed, 0 } * deltaTime);
	if (GetKeyState('L') & 0x8000) setRotation(getRotation() + Vector3{ 0, -cameraSpeed, 0 } * deltaTime);
	if (GetKeyState('I') & 0x8000) setRotation(getRotation() + Vector3{ cameraSpeed, 0, 0 } * deltaTime);
	if (GetKeyState('K') & 0x8000) setRotation(getRotation() + Vector3{ -cameraSpeed, 0, 0 } * deltaTime);


	if (GetKeyState(VK_SPACE) & 0x8000) setPosition(getPosition() + Vector3{ 0, movementSpeed, 0 } *deltaTime);
	if (GetKeyState(VK_LCONTROL) & 0x8000) setPosition(getPosition() + Vector3{ 0, -movementSpeed, 0 } *deltaTime);
}