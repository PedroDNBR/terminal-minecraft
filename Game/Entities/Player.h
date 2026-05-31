#pragma once
#include "../../Renderer/Vector.h"
#include "../../World/Camera.h"
class Player
{
public:
	Player(Camera& camera) : position{ 0, 0, 0 }, rotation{ 0, 0, 0 }, camera(camera) {}

	Vector3 getPosition() const { return position; }
	Vector3 getRotation() const { return rotation; }
	Camera& getCamera() const { return camera; }

	void setPosition(const Vector3& newPosition);
	void setRotation(const Vector3& newRotation);

private:
	Vector3 position;
	Vector3 rotation;

	Camera& camera;

};

