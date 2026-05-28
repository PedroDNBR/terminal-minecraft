#pragma once
#include "../Renderer/Vector.h"

struct Camera
{
	Vector3 position = { 0, 0, 0 };
	float pitch = 0, yaw = 0;
	float fov = 80;

	float cosYaw = 0;
	float sinYaw = 0;
	float cosPitch = 0;
	float sinPitch = 0;

	float fovRadius = 0;
	float focalLen = 0;

	const float nearPlane = .1f;

	void updateCatheti()
	{
		cosYaw = cosf(yaw);
		sinYaw = sinf(yaw);

		cosPitch = cosf(pitch);
		sinPitch = sinf(pitch);
	}
};