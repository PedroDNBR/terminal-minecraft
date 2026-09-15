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

void Player::tick(float deltaTime, InputManager& inputManager)
{
	if (inputManager.isActionDown(InputAction::MOVE_FORWARD)) setPosition(getPosition() + getCamera().getForward() * movementSpeed * deltaTime);
	if (inputManager.isActionDown(InputAction::MOVE_BACKWARD)) setPosition(getPosition() + getCamera().getForward() * -movementSpeed * deltaTime);
	if (inputManager.isActionDown(InputAction::MOVE_RIGHT)) setPosition(getPosition() + getCamera().getRight() * movementSpeed * deltaTime);
	if (inputManager.isActionDown(InputAction::MOVE_LEFT)) setPosition(getPosition() + getCamera().getRight() * -movementSpeed * deltaTime);

	if (inputManager.isActionDown(InputAction::LOOK_LEFT)) setRotation(getRotation() + Vector3{ 0, cameraSpeed, 0 } *deltaTime);
	if (inputManager.isActionDown(InputAction::LOOK_RIGHT)) setRotation(getRotation() + Vector3{ 0, -cameraSpeed, 0 } *deltaTime);
	if (inputManager.isActionDown(InputAction::LOOK_UP)) setRotation(getRotation() + Vector3{ cameraSpeed, 0, 0 } *deltaTime);
	if (inputManager.isActionDown(InputAction::LOOK_DOWN)) setRotation(getRotation() + Vector3{ -cameraSpeed, 0, 0 } *deltaTime);

	if (inputManager.isActionDown(InputAction::JUMP)) setPosition(getPosition() + Vector3{ 0, movementSpeed, 0 } *deltaTime);
	if (inputManager.isActionDown(InputAction::CROUCH)) setPosition(getPosition() + Vector3{ 0, -movementSpeed, 0 } *deltaTime);
	//if (GetKeyState(VK_LCONTROL) & 0x8000) setPosition(getPosition() + Vector3{ 0, -movementSpeed, 0 } *deltaTime);
}