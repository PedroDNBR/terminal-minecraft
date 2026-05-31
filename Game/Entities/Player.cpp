#include "Player.h"

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
