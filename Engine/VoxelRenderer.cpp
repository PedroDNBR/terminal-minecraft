#include "VoxelRenderer.h"
#include "../Renderer/Quad.h"

void VoxelRenderer::render(Renderer& renderer, Camera& camera)
{
	renderer.drawPixel(2, 2, 15);

	Frustum frustum = buildFrustum(camera, renderer.getAspectRatio(), camera.fovRadius);

	for (const auto& chunk : chunksMeshesByPosition)
	{
		float minX = chunk.first.x * (float)Chunk::SIZE_X;
		float minZ = chunk.first.z * (float)Chunk::SIZE_Z;
		float maxX = minX + Chunk::SIZE_X;
		float maxZ = minZ + Chunk::SIZE_Z;

		if (frustum.isBoxOutside(minX, 0.f, minZ, maxX, 64.f, maxZ))
			continue;

		for (const auto& quad : chunk.second)
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
	}

	renderer.drawPixel(2, 5, 10);
}

void VoxelRenderer::unloadMeshes(ChunkManager& chunkManager)
{
	for (const ChunkCoord& coord : chunkManager.meshUnload)
		chunksMeshesByPosition.erase(coord);

	chunkManager.meshUnload.clear();
}

Vector3 VoxelRenderer::convertToCameraSpace(const Vector3& position, Camera& camera)
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

Vertex VoxelRenderer::projectViewSpacePoint(const Vector3& position, Camera& camera, float aspectRatio, int width, int height)
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

Frustum VoxelRenderer::buildFrustum(const Camera& camera, float aspectRatio, float fovRadius)
{
	Frustum frustum;

	float cosYaw = cosf(camera.yaw);
	float sinYaw = sinf(camera.yaw);
	float cosPitch = cosf(camera.pitch);
	float sinPitch = sinf(camera.pitch);

	Vector3 forward = { -sinYaw * cosPitch, sinPitch, cosYaw * cosPitch };
	Vector3 right = { cosYaw, 0, sinYaw };
	Vector3 up = { sinYaw * sinPitch, cosPitch, -cosYaw * cosPitch };

	float horizontalFov = fovRadius * 0.5f * FRUSTUM_PADDING;
	float verticalFov = atanf(tanf(fovRadius * 0.5f) / aspectRatio) * FRUSTUM_PADDING;
	float sinHorizontal = sinf(horizontalFov);
	float cosHorizontal = cosf(horizontalFov);
	float sinVertical = sinf(verticalFov);
	float cosVertical = cosf(verticalFov);

	frustum.planes[0] = makePlane({
		forward.x * cosHorizontal + right.x * sinHorizontal,
		forward.y * cosHorizontal + right.y * sinHorizontal, 
		forward.z * cosHorizontal + right.z * sinHorizontal
	}, camera);

	frustum.planes[1] = makePlane({
		forward.x * cosHorizontal - right.x * sinHorizontal,
		forward.y * cosHorizontal - right.y * sinHorizontal,
		forward.z * cosHorizontal - right.z * sinHorizontal
	}, camera);

	frustum.planes[2] = makePlane({
		forward.x * cosVertical - up.x * sinVertical,
		forward.y * cosVertical - up.y * sinVertical,
		forward.z * cosVertical - up.z * sinVertical
	}, camera);

	frustum.planes[3] = makePlane({
		forward.x * cosVertical - up.x * sinVertical,
		forward.y * cosVertical - up.y * sinVertical,
		forward.z * cosVertical - up.z * sinVertical
	}, camera);

	frustum.planes[4] = { 0, 0, 0, 1 };
	frustum.planes[5] = { 0, 0, 0, 1 };

	return frustum;
}

Plane VoxelRenderer::makePlane(Vector3 normal, const Camera& camera)
{
	float d = -(normal.x * camera.position.x + normal.y * camera.position.y + normal.z * camera.position.z);
	return { normal.x, normal.y, normal.z, d };
}

void VoxelRenderer::meshBuilderWorker(ChunkManager& chunkManager)
{
	while (chunkManager.running)
	{
		ChunkCoord coord;
		{
			std::unique_lock<std::mutex> lock(chunkManager.meshingQueueMutex);
			chunkManager.meshingQueueCV.wait(lock, [&] {
				return !chunkManager.meshingQueue.empty() || !chunkManager.running;
			});
			if (!chunkManager.running) break;
			coord = chunkManager.meshingQueue.front();
			chunkManager.meshingQueue.pop();
		}

		std::shared_ptr<Chunk> chunk, negativeX, positiveX, negativeZ, positiveZ;
		{
			std::shared_lock<std::shared_mutex> rlock(chunkManager.chunksMutex);
			auto iteration = chunkManager.chunks.find(coord);
			if (iteration == chunkManager.chunks.end())
				continue;
			chunk = iteration->second;
			negativeX = chunkManager.getChunkSharedPtr({ coord.x - 1, coord.z });
			positiveX = chunkManager.getChunkSharedPtr({ coord.x + 1, coord.z });
			negativeZ = chunkManager.getChunkSharedPtr({ coord.x, coord.z - 1 });
			positiveZ = chunkManager.getChunkSharedPtr({ coord.x, coord.z + 1 });
		}

		auto quads = buildMeshData(chunk.get(), chunkManager);
		{
			std::lock_guard<std::mutex> rlock(readyMeshesMutex);
			readyMeshes.push({ coord, std::move(quads) });
		}
	}
}

void VoxelRenderer::commitReadyMeshes()
{
	std::lock_guard<std::mutex> lock(readyMeshesMutex);
	while (!readyMeshes.empty())
	{
		ReadyMesh& readyMesh = readyMeshes.front();
		chunksMeshesByPosition[readyMesh.coord] = std::move(readyMesh.quads);
		readyMeshes.pop();
	}
}

std::vector<Quad> VoxelRenderer::buildMeshData(Chunk* chunk, ChunkManager& chunkManager)
{
	std::vector<Quad> chunkQuads;
	chunkQuads.reserve(512);

	for (int x = 0; x < Chunk::SIZE_X; x++)
	for (int z = 0; z < Chunk::SIZE_Z; z++)
	for (int y = 0; y < Chunk::SIZE_Y; y++)
	{
		BlockType blockType = chunk->blocks[x][y][z];
		if (blockType == AIR || blockType == WATER)
			continue;

		Vector3Int blockPosition = { x,y,z };

		for (int f = 0; f < 6; f++)
		{
			if (!chunkManager.isTransparent(chunk, blockPosition + cubeFacesDirections[f]))
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
			quad.color = chunkManager.blockProperties[blockType].faceColors[f];
			chunkQuads.push_back(quad);
		}
	}
	return chunkQuads;
}
