#pragma once
#include "../Core/Vector.h"

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

	Vector3 getForward() const
	{
		return {
			-sin(yaw), 
			0, 
			cos(yaw)
		};
	}
	
	Vector3 getRight() const
	{
		Vector3 forward = getForward();
		return {
			forward.z,
			0,
			-forward.x
		};
	}

	void updateCatheti()
	{
		cosYaw = cosf(yaw);
		sinYaw = sinf(yaw);

		cosPitch = cosf(pitch);
		sinPitch = sinf(pitch);
	}
};