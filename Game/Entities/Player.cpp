#include "Player.h"
#include "../../World/World.h"
#include "../../World/Chunk/BlockProperties.h"

void Player::setPosition(const Vector3& newPosition)
{
	position = newPosition;
	camera.position = position + Vector3{ 0, currentEyesHeight(), 0 };
}

void Player::setRotation(const Vector3& newRotation)
{
	rotation = newRotation;
	camera.yaw = rotation.y * 3.14159f / 180;
	camera.pitch = rotation.x * 3.14159f / 180;
}

void Player::tick(float deltaTime, InputManager& inputManager, World& world)
{
	if (inputManager.isActionPressed(InputAction::OPEN_INVENTORY))
		showInventory = !showInventory;


	if (showInventory)
		handleInventoryManagement(inputManager);


	if (!showInventory)
		handleCameraRotation(deltaTime, inputManager);

	switch (gameMode)
	{
		case GameMode::Creative:
			if (!showInventory)
				handleCreativeMovement(deltaTime, inputManager);
			break;
		case GameMode::Survival:
			if (!showInventory)
			{
				isCrouched = inputManager.isActionDown(InputAction::CROUCH);
				isSprinting = inputManager.isActionDown(InputAction::SPRINT);

				if (inputManager.isActionDown(InputAction::JUMP))
					requestJump = true;
			}
			else
			{
				isCrouched = false;
				isSprinting = false;
				requestJump = false;
			}

				
			break;
	}

	if (!showInventory)
		handleBlockManagement(deltaTime, inputManager, world);

	if (!showInventory)
		if (inputManager.isActionPressed(InputAction::CHANGE_GAMEMODE))
		{
			switch (gameMode)
			{
			case GameMode::Survival:
				gameMode = GameMode::Creative;
				velocity = Vector3{ 0, 0, 0 };
				break;
			case GameMode::Creative:
				gameMode = GameMode::Survival;
				break;
			}
		}

	if (inputManager.isActionPressed(InputAction::HOTBAR_1))
		currentHotbarSlotSelected = 0;
	if (inputManager.isActionPressed(InputAction::HOTBAR_2))
		currentHotbarSlotSelected = 1;
	if (inputManager.isActionPressed(InputAction::HOTBAR_3))
		currentHotbarSlotSelected = 2;
	if (inputManager.isActionPressed(InputAction::HOTBAR_4))
		currentHotbarSlotSelected = 3;
	if (inputManager.isActionPressed(InputAction::HOTBAR_5))
		currentHotbarSlotSelected = 4;
	if (inputManager.isActionPressed(InputAction::HOTBAR_6))
		currentHotbarSlotSelected = 5;
	if (inputManager.isActionPressed(InputAction::HOTBAR_7))
		currentHotbarSlotSelected = 6;
	if (inputManager.isActionPressed(InputAction::HOTBAR_8))
		currentHotbarSlotSelected = 7;
	if (inputManager.isActionPressed(InputAction::HOTBAR_9))
		currentHotbarSlotSelected = 8;

}

void Player::fixedTick(float fixedDeltaTime, InputManager& inputManager, World& world)
{
	switch (gameMode)
	{
	case GameMode::Survival:
		handleSurvivalMovement(fixedDeltaTime, inputManager, world);
		break;
	}
}

void Player::addItemToInventory(BlockType blockType, uint8_t count)
{
	for (int slot = 0; slot < TOTAL_SLOTS; slot++)
	{
		if (
			!inventory[slot].isEmpty() &&
			inventory[slot].type == blockType &&
			inventory[slot].count < MAX_ITEM_STACK
			)
		{
			inventory[slot].count++;
			return;
		}
	}

	for (int slot = 0; slot < TOTAL_SLOTS; slot++)
	{
		if (inventory[slot].isEmpty())
		{
			inventory[slot].type = blockType;
			inventory[slot].count = 1;
			return;
		}
	}
}

void Player::addItemToInventory(BlockType blockType, uint8_t count, uint8_t slot)
{
	if (slot >= inventory.size())
		return;

	inventory[slot].type = blockType;
	inventory[slot].count = count;
}


void Player::removeItemFromInventory(uint8_t slot)
{
	if (slot >= inventory.size())
		return;
	inventory[slot].type = B_AIR;
	inventory[slot].count = 0;
}

void Player::selectHotbarSlot(uint8_t slot)
{
	currentHotbarSlotSelected = slot;
}

void Player::handleSurvivalMovement(float deltaTime, InputManager& inputManager, World& world)
{

	Vector3 targetMovementDirection = {};
	if (!showInventory)
	{
		if (inputManager.isActionDown(InputAction::MOVE_FORWARD)) targetMovementDirection += getCamera().getForward();
		if (inputManager.isActionDown(InputAction::MOVE_BACKWARD)) targetMovementDirection -= getCamera().getForward();
		if (inputManager.isActionDown(InputAction::MOVE_RIGHT)) targetMovementDirection += getCamera().getRight();
		if (inputManager.isActionDown(InputAction::MOVE_LEFT)) targetMovementDirection -= getCamera().getRight();
	}
	targetMovementDirection = Vector3::normalize(targetMovementDirection);

	float speed = isSprinting ? WALK_SPEED * SPRINT_MULTIPLIER : WALK_SPEED;
	speed = (isCrouched) ? WALK_SPEED * CROUCH_MULTIPLIER : speed;
	velocity.x = targetMovementDirection.x * speed;
	velocity.z = targetMovementDirection.z * speed;

	if (requestJump && isGrounded)
		velocity.y = JUMP_FORCE;

	requestJump = false;

	velocity.y -= world.GRAVITY * deltaTime;
	if (velocity.y < -TERMINAL_VELOCITY)
		velocity.y = -TERMINAL_VELOCITY;
	else if (velocity.y > TERMINAL_VELOCITY)
		velocity.y = TERMINAL_VELOCITY;

	moveWithCollision(velocity * deltaTime, world);
}

void Player::handleCreativeMovement(float deltaTime, InputManager& inputManager)
{
	float speed = (inputManager.isActionDown(InputAction::SPRINT)) ? WALK_SPEED * 2.0f : WALK_SPEED;

	if (inputManager.isActionDown(InputAction::MOVE_FORWARD)) setPosition(getPosition() + getCamera().getForward() * speed * deltaTime);
	if (inputManager.isActionDown(InputAction::MOVE_BACKWARD)) setPosition(getPosition() + getCamera().getForward() * -speed * deltaTime);
	if (inputManager.isActionDown(InputAction::MOVE_RIGHT)) setPosition(getPosition() + getCamera().getRight() * speed * deltaTime);
	if (inputManager.isActionDown(InputAction::MOVE_LEFT)) setPosition(getPosition() + getCamera().getRight() * -speed * deltaTime);

	if (inputManager.isActionDown(InputAction::JUMP)) setPosition(getPosition() + Vector3{ 0, speed, 0 } *deltaTime);
	if (inputManager.isActionDown(InputAction::CROUCH)) setPosition(getPosition() + Vector3{ 0, -speed, 0 } *deltaTime);
}

void Player::handleCameraRotation(float deltaTime, InputManager& inputManager)
{
	if (inputManager.isActionDown(InputAction::LOOK_LEFT)) setRotation(getRotation() + Vector3{ 0, CAMERA_LOOK_SPEED, 0 } *deltaTime);
	if (inputManager.isActionDown(InputAction::LOOK_RIGHT)) setRotation(getRotation() + Vector3{ 0, -CAMERA_LOOK_SPEED, 0 } *deltaTime);
	if (inputManager.isActionDown(InputAction::LOOK_UP)) setRotation(getRotation() + Vector3{ CAMERA_LOOK_SPEED, 0, 0 } *deltaTime);
	if (inputManager.isActionDown(InputAction::LOOK_DOWN)) setRotation(getRotation() + Vector3{ -CAMERA_LOOK_SPEED, 0, 0 } *deltaTime);
}

void Player::handleBlockManagement(float deltaTime, InputManager& inputManager, World& world)
{
	if (inputManager.isActionPressed(InputAction::PLACE_BLOCK))
	{
		placeSelectedBlockOnSight(world);
	}
	if (inputManager.isActionPressed(InputAction::DESTROY_BLOCK))
	{
		destroyBlockOnSight(world);
	}
}

void Player::handleInventoryManagement(InputManager& inputManager)
{
	if (inputManager.isActionPressed(InputAction::UI_LEFT))
		currentInventoryColumnSlotHovered = (currentInventoryColumnSlotHovered + INVENTORY_COLUMNS - 1) % INVENTORY_COLUMNS;

	if (inputManager.isActionPressed(InputAction::UI_RIGHT))
		currentInventoryColumnSlotHovered = (currentInventoryColumnSlotHovered + 1) % INVENTORY_COLUMNS;

	if (inputManager.isActionPressed(InputAction::UI_UP))
		currentInventoryRowSlotHovered = (currentInventoryRowSlotHovered + INVENTORY_TOTAL_ROWS - 1) % INVENTORY_TOTAL_ROWS;

	if (inputManager.isActionPressed(InputAction::UI_DOWN))
		currentInventoryRowSlotHovered = (currentInventoryRowSlotHovered + 1) % INVENTORY_TOTAL_ROWS;

	if (inputManager.isActionPressed(InputAction::UI_SELECT))
	{
		if(currentInventorySlotSelected == NO_SLOT_SELECTED)
			currentInventorySlotSelected = hoveredSlotFromGrid();
		else
		{
			std::swap(inventory[currentInventorySlotSelected], inventory[hoveredSlotFromGrid()]);
			currentInventorySlotSelected = NO_SLOT_SELECTED;
		}
	}

	if (inputManager.isActionPressed(InputAction::UI_CANCEL))
		currentInventorySlotSelected = NO_SLOT_SELECTED;
}

void Player::placeSelectedBlockOnSight(World& world)
{
	RaycastHit hit = world.raycast(getCamera().position, getCamera().getLookDirection(), BLOCK_REACH);

	if (!hit.hit) return;
	if (occupiesCell(hit.placement)) return;
	if (getHotbar()[currentHotbarSlotSelected].count == 0) return;

	world.setBlockAtWorldPosition(hit.placement, getHotbar()[currentHotbarSlotSelected].type);
	if (gameMode == GameMode::Survival)
	{
		inventory[currentHotbarSlotSelected].count--;
		if (getHotbar()[currentHotbarSlotSelected].count == 0)
			removeItemFromInventory(currentHotbarSlotSelected);
	}
}

void Player::destroyBlockOnSight(World& world)
{
	RaycastHit hit = world.raycast(getCamera().position, getCamera().getLookDirection(), BLOCK_REACH);
	if (hit.hit)
	{
		world.setBlockAtWorldPosition(hit.block, static_cast<BlockType>(B_AIR));
		if (gameMode == GameMode::Survival)
			addItemToInventory(hit.blockType, 1);
	}
}

void Player::moveWithCollision(const Vector3& displacement, const World& world)
{
	float longest = fmaxf(
		fabsf(displacement.x),
		fmaxf(
			fabsf(displacement.y), 
			fabsf(displacement.z)
		)
	);

	int steps = (int)ceilf(longest / MAX_STEP);
	if (steps < 1) steps = 1;

	Vector3 slice = displacement * (1.0f / steps);

	for(int i = 0; i < steps; i++)
	{
		Vector3 newPosition = getPosition();

		Vector3 tryY = newPosition;  
		tryY.y += slice.y;
		if (collideWithWorld(tryY, world))
		{
			if (slice.y < 0)
				newPosition.y = std::floorf(tryY.y) + 1.0f;
			else
				newPosition.y = std::floorf(tryY.y + currentHeight()) - currentHeight() - EPSILON;

			velocity.y = 0;
			slice.y = 0;
		}
		else
			newPosition.y = tryY.y;

		Vector3 tryX = newPosition;
		tryX.x += slice.x;
		if (!collideWithWorld(tryX, world) && !(isCrouched && isGrounded && !collideWithWorld(tryX - Vector3(0, .1f, 0), world)))
			newPosition.x = tryX.x;
		else
		{
			velocity.x = 0;
			slice.x = 0;
		}

		Vector3 tryZ = newPosition;
		tryZ.z += slice.z;
		if (!collideWithWorld(tryZ, world) && !(isCrouched && isGrounded && !collideWithWorld(tryZ - Vector3(0, .1f, 0), world)))
			newPosition.z = tryZ.z;
		else
		{
			velocity.z = 0;
			slice.z = 0;
		}

		setPosition(newPosition);
	}

	isGrounded = collideWithWorld(getPosition() - Vector3(0, GROUNDED_PROBE, 0), world);
}

bool Player::collideWithWorld(const Vector3& newPosition, const World& world) const
{
	CellRange range = occupiedCells(newPosition);
	for (int x = range.min.x; x <= range.max.x; x++)
		for (int y = range.min.y; y <= range.max.y; y++)
			for (int z = range.min.z; z <= range.max.z; z++)
				if (Blocks::DENSITY[world.getBlockAtWorldPosition({ x,y,z })] == Blocks::Density::SOLID)
					return true;
	return false;
}

bool Player::occupiesCell(Vector3Int cell) const
{
	CellRange range = occupiedCells(getPosition());
	return cell.x >= range.min.x && cell.x <= range.max.x
		&& cell.y >= range.min.y && cell.y <= range.max.y
		&& cell.z >= range.min.z && cell.z <= range.max.z;
}

CellRange Player::occupiedCells(const Vector3& feetPosition) const
{
	return {
		{ (int)floorf(feetPosition.x - PLAYER_HALF_WIDTH),
		  (int)floorf(feetPosition.y),
		  (int)floorf(feetPosition.z - PLAYER_HALF_WIDTH) },

		{ (int)ceilf(feetPosition.x + PLAYER_HALF_WIDTH) - 1,
		  (int)ceilf(feetPosition.y + currentHeight()) - 1,
		  (int)ceilf(feetPosition.z + PLAYER_HALF_WIDTH) - 1 }
	};
}
