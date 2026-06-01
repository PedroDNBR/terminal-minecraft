#include "VoxelRenderer.h"
#include "../Renderer/Quad.h"

void VoxelRenderer::render(Renderer& renderer, Camera& camera)
{
	renderer.drawPixel(2, 2, 15);

	for (auto chunk : chunksMeshesByPosition)
	for (auto quad : chunk.second)
	{
		renderer.drawPixel(2, 3, 4);
		Vector3 center = (quad.v0 + quad.v1 + quad.v2 + quad.v3) * .25f;
		Vector3 toCameraView = camera.position - center;
		float dot = quad.normal.x * toCameraView.x +
			quad.normal.y * toCameraView.y +
			quad.normal.z * toCameraView.z;

		if (dot <= 0.0f)
			continue;


		Vector3 pointsRelativePositions[4] = {
			convertToCameraSpace(quad.v0, camera),
			convertToCameraSpace(quad.v1, camera),
			convertToCameraSpace(quad.v2, camera),
			convertToCameraSpace(quad.v3, camera)
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
			renderer.drawFilledQuad(projected[0], projected[i], projected[i + 1], projected[i + 1], quad.color);
	}

	renderer.drawPixel(2, 5, 10);
}

void VoxelRenderer::generateChunkMesh(Chunk* chunk, BlockProperties* blockProperties, TerrainGenerator* terrainGenerator)
{
	std::vector<Quad> chunkQuads;
	chunkQuads.reserve(512);

	for (int x = 0; x < Chunk::SIZE_X; x++)
	for (int z = 0; z < Chunk::SIZE_Z; z++)
	for (int y = 0; y < Chunk::SIZE_Y; y++)
	{
		Vector3Int blockPosition = { x,y,z };
		for (int f = 0; f < 6; f++)
		{
			BlockType blockType = chunk->blocks[x][y][z];
			if (blockType == AIR || blockType == WATER)
				continue;

			if (!terrainGenerator->isTransparent(chunk, blockPosition + cubeFacesDirections[f]))
				continue;

			Quad quad = {};
			Vector3 positionOffset = {
				(float)x + (float)chunk->position.x * Chunk::SIZE_X,
				(float)y,
				(float)z + (float)chunk->position.z * Chunk::SIZE_Z };

			quad.v0 = cubeVerticesPositions[faceIndices[f][0]] + positionOffset;
			quad.v1 = cubeVerticesPositions[faceIndices[f][1]] + positionOffset;
			quad.v2 = cubeVerticesPositions[faceIndices[f][2]] + positionOffset;
			quad.v3 = cubeVerticesPositions[faceIndices[f][3]] + positionOffset;
			quad.normal = cubeFacesDirections[f];
			quad.color = blockProperties[blockType].faceColors[f];
			chunkQuads.push_back(quad);
		}
	}
	

	chunksMeshesByPosition[chunk->position] = chunkQuads;
}

Vector3 VoxelRenderer::convertToCameraSpace(Vector3& position, Camera& camera)
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

Vertex VoxelRenderer::projectViewSpacePoint(Vector3& position, Camera& camera, float aspectRatio, int width, int height)
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

int VoxelRenderer::clipNearPlane(Vector3* source, int vertexCount, Vector3* output, Camera& camera)
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
