#include "Player.h"

void Player::setPosition(const Vector3& newPosition)
{
	position = newPosition;
	camera.position = position;
}

void Player::setRotation(const Vector3& newRotation)
{
	rotation = newRotation;
	camera.yaw = rotation.y;
	camera.pitch = rotation.x;
}
