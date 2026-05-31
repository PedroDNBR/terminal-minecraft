#include "VoxelEngine.h"
#include "../Renderer/Quad.h"

void VoxelEngine::render(Renderer& renderer, Camera& camera)
{
	renderer.drawPixel(2, 2, 15);

	Quad cubeFaces[6];

	for (int f = 0; f < 6; f++)
	{
		cubeFaces[f].v0 = cubeVerticesPositions[faceIndices[f][0]];
		cubeFaces[f].v1 = cubeVerticesPositions[faceIndices[f][1]];
		cubeFaces[f].v2 = cubeVerticesPositions[faceIndices[f][2]];
		cubeFaces[f].v3 = cubeVerticesPositions[faceIndices[f][3]];
		cubeFaces[f].normal = cubeFacesDirections[f];
		cubeFaces[f].color = 3;
	}

	for (int q = 0; q < 6; q++)
	{
		renderer.drawPixel(2, 3, 4);
		Vector3 center = (cubeFaces[q].v0 + cubeFaces[q].v1 + cubeFaces[q].v2 + cubeFaces[q].v3) * .25f;
		Vector3 toCameraView = camera.position - center;
		float dot = cubeFaces[q].normal.x * toCameraView.x +
			cubeFaces[q].normal.y * toCameraView.y +
			cubeFaces[q].normal.z * toCameraView.z;

		if (dot <= 0.0f)
			continue;


		Vector3 pointsRelativePositions[4] = {
			convertToCameraSpace(cubeFaces[q].v0, camera),
			convertToCameraSpace(cubeFaces[q].v1, camera),
			convertToCameraSpace(cubeFaces[q].v2, camera),
			convertToCameraSpace(cubeFaces[q].v3, camera)
		};

		Vector3 clipped[6];
		int clippedCount = clipNearPlane(pointsRelativePositions, 4, clipped, camera);

		if (clippedCount < 3)
			continue;

		renderer.drawPixel(2, 4, 14);

		Vertex projected[6];
		for (int i = 0; i < clippedCount; i++)
			projected[i] = projectViewSpacePoint(clipped[i], camera, renderer.getAspectRatio(), renderer.getLogicalWidth(), renderer.getLogicalHeight());

		for (int i = 1; i + 1 < clippedCount; i++)
			renderer.drawFilledQuad(projected[0], projected[i], projected[i + 1], projected[i + 1], cubeFaces[q].color + q);
	}

	renderer.drawPixel(2, 5, 10);
}

Vector3 VoxelEngine::convertToCameraSpace(Vector3& position, Camera& camera)
{
	Vector3 relativePosition = position - camera.position;
	float relativeX = camera.cosYaw * relativePosition.x + camera.sinYaw * relativePosition.z;
	float relativeZ = -camera.sinYaw * relativePosition.x + camera.cosYaw * relativePosition.z;
	float relativeY = camera.cosPitch * relativePosition.y - camera.sinPitch * relativeZ;
	relativeZ = camera.sinPitch * relativePosition.y + camera.cosPitch * relativeZ;
	
	relativePosition.x = relativeX;
	relativePosition.y = relativeY;
	relativePosition.z = relativeZ;

	return relativePosition;
}

Vertex VoxelEngine::projectViewSpacePoint(Vector3& position, Camera& camera, float aspectRatio, int width, int height)
{
	float inverseZ = 1.f / position.z;
	return {
		{
			(position.x * camera.focalLen * inverseZ) * height * aspectRatio + width * .5f,
			-(position.y * camera.focalLen * inverseZ) * height + height * .5f,
			position.z
		},
		inverseZ 
	};
}

int VoxelEngine::clipNearPlane(Vector3* source, int vertexCount, Vector3* output, Camera& camera)
{
	int outputCount = 0;
	for (int i = 0; i < vertexCount; i++)
	{
		Vector3& a = source[i];
		Vector3& b = source[(i + 1) % vertexCount];
		bool isAInside = a.z >= camera.nearPlane;
		bool isBInside = b.z >= camera.nearPlane;
		if (isAInside)
			output[outputCount++] = a;
		if (isAInside != isBInside)
		{
			float interpolation = (camera.nearPlane - a.z) / (b.z - a.z);
			output[outputCount++] = {
				a.x + interpolation * (b.x - a.x),
				a.y + interpolation * (b.y - a.y),
				camera.nearPlane
			};
		}
	}
	return outputCount;
}
