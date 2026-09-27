#pragma once
#include "../InputManager.h"
#include "../../Core/Vector.h"
#include "../../World/Camera.h"
#include "../../World/World.h"
#include "../GameMode.h"
#include "../Inventory/InventorySlot.h"

struct CellRange { Vector3Int min, max; };

class Player
{
private:
	static constexpr float WALK_SPEED = 4.f;
	static constexpr float CAMERA_LOOK_SPEED = 80.f;
	static constexpr float SPRINT_MULTIPLIER = 1.75f;
	static constexpr float CROUCH_MULTIPLIER = 0.6f;

	static constexpr float JUMP_FORCE = 8.f;

	static constexpr float TERMINAL_VELOCITY = 80.f;

	static constexpr float PLAYER_HALF_WIDTH = 0.3f;
	static constexpr float PLAYER_HEIGHT = 1.8f;
	static constexpr float EYE_HEIGHT = 1.65f;
	static constexpr float PLAYER_CROUCHED_HEIGHT = 1.4f;
	static constexpr float EYE_CROUCHED_HEIGHT = 1.25f;

	static constexpr float EPSILON = 0.0001f;

	static constexpr float GROUNDED_PROBE = 0.01f;

	static constexpr float MAX_STEP = 0.2f;

	static constexpr int MAX_HOTBAR_SLOTS = 9;

	static constexpr int MAX_ITEM_STACK = 64;

public:
	Player(Camera& camera) : position{ 0, 0, 0 }, rotation{ 0, 0, 0 }, camera(camera) {}

	Vector3 getPosition() const { return position; }
	Vector3 getRotation() const { return rotation; }
	float getCameraYaw() const { return camera.yaw; }
	float getCameraPitch() const { return camera.pitch; }
	Camera& getCamera() const { return camera; }

	void setPosition(const Vector3& newPosition);
	void setRotation(const Vector3& newRotation);
	void tick(float deltaTime, InputManager& inputManager, World& world);
	void fixedTick(float fixedDeltaTime, InputManager& inputManager, World& world);

	void addItemToHotbar(BlockType blockType, uint8_t count, uint8_t slot);
	void removeItemFromHotbar(uint8_t slot);

	void selectHotbarSlot(uint8_t slot);

	std::array<InventorySlot, MAX_HOTBAR_SLOTS> getHotbar() const& { return hotbar; }
	uint8_t getCurrentHotbarSlotSelected() const& { return currentHotbarSlotSelected; }
	BlockType getCurrentHotbarSlotBlockType() const& { return hotbar[currentHotbarSlotSelected].type; }
	BlockType getHotbarSlotBlockTypeByIndex(uint8_t index) const& { return hotbar[index].type; }

private:
	bool isCrouched = false;
	bool isSprinting = false;
	bool isGrounded = false;

	bool requestJump = false;

	Vector3 position;
	Vector3 rotation;

	Vector3 velocity;

	Camera& camera;

	const float BLOCK_REACH = 5.f;

	GameMode gameMode = GameMode::Survival;

	std::array<InventorySlot, MAX_HOTBAR_SLOTS> hotbar;

	uint8_t currentHotbarSlotSelected = 0;

	float currentHeight() const { return isCrouched ? PLAYER_CROUCHED_HEIGHT : PLAYER_HEIGHT; }
	float currentEyesHeight() const { return isCrouched ? EYE_CROUCHED_HEIGHT : EYE_HEIGHT; }

	void handleSurvivalMovement(float deltaTime, InputManager& inputManager, World& world);
	void handleCreativeMovement(float deltaTime, InputManager& inputManager);

	void handleCameraRotation(float deltaTime, InputManager& inputManager);
	void handleBlockManagement(float deltaTime, InputManager& inputManager, World& world);

	void placeSelectedBlockOnSight(World& world);

	void destroyBlockOnSight(World& world);

	void moveWithCollision(const Vector3& displacement, const World& world);

	bool collideWithWorld(const Vector3& newPosition, const World& world) const;
	bool occupiesCell(Vector3Int cell) const;

	CellRange occupiedCells(const Vector3& feetPosition) const;
};

