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
	handleCameraRotation(deltaTime, inputManager);

	switch (gameMode)
	{
		case GameMode::Creative:
			handleCreativeMovement(deltaTime, inputManager);
			break;
		case GameMode::Survival:
			isCrouched = inputManager.isActionDown(InputAction::CROUCH);
			isSprinting = inputManager.isActionDown(InputAction::SPRINT);

			if(inputManager.isActionDown(InputAction::JUMP))
				requestJump = true;
			break;
	}

	handleBlockManagement(deltaTime, inputManager, world);

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

void Player::handleSurvivalMovement(float deltaTime, InputManager& inputManager, World& world)
{

	Vector3 targetMovementDirection = {};
	if (inputManager.isActionDown(InputAction::MOVE_FORWARD)) targetMovementDirection += getCamera().getForward();
	if (inputManager.isActionDown(InputAction::MOVE_BACKWARD)) targetMovementDirection -= getCamera().getForward();
	if (inputManager.isActionDown(InputAction::MOVE_RIGHT)) targetMovementDirection += getCamera().getRight();
	if (inputManager.isActionDown(InputAction::MOVE_LEFT)) targetMovementDirection -= getCamera().getRight();
	targetMovementDirection = Vector3::normalize(targetMovementDirection);

	float speed = isSprinting ? WALK_SPEED * SPRINT_MULTIPLIER : WALK_SPEED;
	speed = (isCrouched) ? WALK_SPEED * CROUCH_MULTIPLIER : speed;
	velocity.x = targetMovementDirection.x * speed;
	velocity.z = targetMovementDirection.z * speed;

	if (requestJump && isGrounded)
		velocity.y = JUMP_FORCE;

	requestJump = false;

	velocity.y -= world.GRAVITY * deltaTime;
	if(velocity.y < -TERMINAL_VELOCITY)
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
		RaycastHit hit = world.raycast(getCamera().position, getCamera().getLookDirection(), BLOCK_REACH);
		if (hit.hit && hit.placement != Vector3Int{ (int)std::floorf(getCamera().position.x), (int)std::floorf(getCamera().position.y), (int)std::floorf(getCamera().position.z) })
			world.setBlockAtWorldPosition(hit.placement, static_cast<BlockType>(B_STONE));
	}
	if (inputManager.isActionPressed(InputAction::DESTROY_BLOCK))
	{
		RaycastHit hit = world.raycast(getCamera().position, getCamera().getLookDirection(), BLOCK_REACH);
		if (hit.hit)
			world.setBlockAtWorldPosition(hit.block, static_cast<BlockType>(B_AIR));
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
	float minX = newPosition.x - PLAYER_HALF_WIDTH;
	float maxX = newPosition.x + PLAYER_HALF_WIDTH;

	float minY = newPosition.y;
	float maxY = newPosition.y + currentHeight();

	float minZ = newPosition.z - PLAYER_HALF_WIDTH;
	float maxZ = newPosition.z + PLAYER_HALF_WIDTH;

	for (int x = (int)floorf(minX); x <= (int)ceilf(maxX) - 1; x++)
	for (int y = (int)floorf(minY); y <= (int)ceilf(maxY) - 1; y++)
	for (int z = (int)floorf(minZ); z <= (int)ceilf(maxZ) - 1; z++)
	{
		if(Blocks::DENSITY[world.getBlockAtWorldPosition({ x, y, z })] == Blocks::Density::SOLID)
			return true;
	}

	return false;
}
