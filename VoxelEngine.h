#pragma once
#include "Renderer.h"
#include "Camera.h"
#include "Vertex.h"
#include "DirectionsXZ.h"

class VoxelEngine
{
public:
	void render(Renderer& renderer, Camera& camera);

private:
	Vector3 convertToCameraSpace(Vector3& position, Camera& camera);
	Vertex projectViewSpacePoint(Vector3& position, Camera& camera, float aspectRatio, int width, int height);
	int clipNearPlane(Vector3* source, int vertexCount, Vector3* output, Camera& camera);

	const Vector3 cubes[4] = {
		{0,0,0},
		{1,0,0},
		{1,0,1},
		{0,0,1}
	};

	const Vector3 cubeVerticesPositions[8] = {
		{0,0,0},
		{1,0,0},
		{1,1,0},
		{0,1,0},
		{0,0,1},
		{1,0,1},
		{1,1,1},
		{0,1,1}
	};

	const Vector3 cubeFacesDirections[6] = {
		{0,0,1},
		{0,0,-1},
		{-1,0,0},
		{1,0,0},
		{0,1,0},
		{0,-1,0}
	};

	const DirectionsXZ neighboursDirections[4] = {
		{ 1,  0},
		{-1,  0},
		{ 0,  1},
		{ 0, -1}
	};
};
