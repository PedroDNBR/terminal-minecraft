#include "Player.h"
#include "../../World/World.h"

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

void Player::tick(float deltaTime, InputManager& inputManager, World& world)
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

	if (inputManager.isActionPressed(InputAction::PLACE_BLOCK))
	{
		RaycastHit hit = world.raycast(getPosition(), getCamera().getLookDirection(), BLOCK_REACH);
		if (hit.hit && hit.placement != Vector3Int{(int)std::floorf(getPosition().x), (int)std::floorf(getPosition().y), (int)std::floorf(getPosition().z)})
			world.setBlockAtWorldPosition(hit.placement, static_cast<BlockType>(B_STONE));
	}
	if (inputManager.isActionPressed(InputAction::DESTROY_BLOCK))
	{
		RaycastHit hit = world.raycast(getPosition(), getCamera().getLookDirection(), BLOCK_REACH);
		if(hit.hit)
			world.setBlockAtWorldPosition(hit.block, static_cast<BlockType>(B_AIR));
	}
}