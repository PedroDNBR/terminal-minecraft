#include "VoxelEngine.h"
#include "../Renderer/Quad.h"

void VoxelEngine::render(Renderer& renderer, Camera& camera)
{
	renderer.drawPixel(2, 2, 15);

	/*for (int i = 0; i < 4; i++)
	{
		Quad cubeFaces[6];

		for (int i = 0; i < 6; i++)
		{

		}
	}*/

	Quad quad[2];

	quad[1].v0 = { -1, -1, 11 };
	quad[1].v1 = { 1, -1, 11 };
	quad[1].v2 = { 1, 1, 11 };
	quad[1].v3 = { -1, 1, 11 };
	quad[1].normal = { 0, 0, -1 };

	quad[0].v0 = { -1, -1, 10 };
	quad[0].v1 = { 1, -1, 10 };
	quad[0].v2 = { 1, 1, 10 };
	quad[0].v3 = { -1, 1, 10 };
	quad[0].normal = { 0, 0, -1 };

	const int width = renderer.getLogicalWidth();
	const int height = renderer.getLogicalHeight();

	for (int q = 0; q < 2; q++)
	{
		// verifica se o quad está virado para a camera
		Vector3 center = (quad[q].v0 + quad[q].v1 + quad[q].v2 + quad[q].v3) * .25f;
		Vector3 toCameraView = camera.position - center;
		float dot = quad[q].normal.x * toCameraView.x +
			quad[q].normal.y * toCameraView.y +
			quad[q].normal.z * toCameraView.z;

		if (dot <= 0.0f)
			return;

		renderer.drawPixel(2, 3, 4);

		Vector3 pointsRelativePositions[4] = {
			convertToCameraSpace(quad[q].v0, camera),
			convertToCameraSpace(quad[q].v1, camera),
			convertToCameraSpace(quad[q].v2, camera),
			convertToCameraSpace(quad[q].v3, camera)
		};

		Vector3 clipped[6];
		int clippedCount = clipNearPlane(pointsRelativePositions, 4, clipped, camera);

		if (clippedCount < 3)
			return;

		renderer.drawPixel(2, 4, 14);

		Vertex projected[6];
		for (int i = 0; i < clippedCount; i++)
			projected[i] = projectViewSpacePoint(clipped[i], camera, renderer.getAspectRatio(), renderer.getLogicalWidth(), renderer.getLogicalHeight());

		for (int i = 0; i < clippedCount; i++)
			renderer.drawPixelDepth(projected[i].viewPosition.x, projected[i].viewPosition.y, 13 + q, projected[i].inverseZ);
	}

	

	renderer.drawPixel(2, 5, 10);

}

Vector3 VoxelEngine::convertToCameraSpace(Vector3& position, Camera& camera)
{
	Vector3 relativePosition = position - camera.position;
	float relativeX = camera.cosYaw * relativePosition.x - camera.sinYaw * relativePosition.z;
	float relativeZ = -camera.sinYaw * relativePosition.x + camera.cosYaw * relativePosition.z;
	relativePosition.x = relativeX;
	relativePosition.z = relativeZ;
	float relativeY = camera.cosPitch * relativePosition.y - camera.sinPitch * relativePosition.z;
	relativePosition.y = relativeY;

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
