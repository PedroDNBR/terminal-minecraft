#include "VoxelRenderer.h"
#include "Quad.h"

void VoxelRenderer::render(Renderer& renderer, Camera& camera, Lighting& lighting)
{
	if (meshListDirty)
	{
		meshDrawList.clear();
		meshDrawList.reserve(chunksMeshesByPosition.size());
		for (const auto& [coord, quads] : chunksMeshesByPosition)
			meshDrawList.push_back({ coord, &quads });
		meshListDirty = false;
	}

	Frustum frustum = buildFrustum(camera, renderer.getAspectRatio(), camera.fovRadius);

	for (const auto& mesh : meshDrawList)
	{
		float minX = mesh.coord.x * (float)Chunk::SIZE_X;
		float minZ = mesh.coord.z * (float)Chunk::SIZE_Z;
		float maxX = minX + Chunk::SIZE_X;
		float maxZ = minZ + Chunk::SIZE_Z;

		if (frustum.isBoxOutside(minX, 0.f, minZ, maxX, 64.f, maxZ))
			continue;

		for (const auto& quad : *mesh.quads)
		{
			Vector3Int direction = cubeFacesDirections[quad.faceIndex];

			int normalAxis = (direction.x != 0) ? 0 : (direction.y != 0) ? 1 : 2;

			int uAxis = (normalAxis == 0) ? 1 : 0;
			int vAxis = (normalAxis == 2) ? 1 : 2;

			Vector3 center = reconstructCenter(quad, minX, minZ, normalAxis, uAxis, vAxis);

			Vector3 toCameraView = camera.position - center;

			float dot = direction.x * (camera.position.x - center.x)
				+ direction.y * (camera.position.y - center.y)
				+ direction.z * (camera.position.z - center.z);

			if (dot <= 0.0f)
				continue;

			Vector3 v0, v1, v2, v3;
			reconstructQuad(quad, minX, minZ, normalAxis, uAxis, vAxis, v0, v1, v2, v3);

			Vector3 pointsRelativePositions[4] = {
				convertToCameraSpace(v0, camera),
				convertToCameraSpace(v1, camera),
				convertToCameraSpace(v2, camera),
				convertToCameraSpace(v3, camera)
			};

			Vector3 clipped[6];
			int clippedCount = clipNearPlane(pointsRelativePositions, 4, clipped, camera);

			if (clippedCount < 3)
				continue;

			Vertex projected[6];
			for (int i = 0; i < clippedCount; i++)
				projected[i] = projectViewSpacePoint(clipped[i], camera, renderer.getAspectRatio(), renderer.getLogicalWidth(), renderer.getLogicalHeight());

            int skyReaching = (std::min)((int)quad.skyLight, lighting.globalLight);
			//int shade = skyReaching + FACE_SHAPE[quad.faceIndex] + lighting.faceShading[quad.faceIndex];
			int shade = (std::min)((int)quad.skyLight, lighting.globalLight) + FACE_SHAPE[quad.faceIndex] + lighting.faceShading[quad.faceIndex];
			shade = (std::max)(shade, (int)(skyReaching * .33f));
			uint8_t shadedColorIndex = colorIndex((Color)quad.color, shade);

			for (int i = 1; i + 1 < clippedCount; i++)
				renderer.drawFilledQuad(projected[0], projected[i], projected[i + 1], projected[i + 1], shadedColorIndex);
		}
	}
}

void VoxelRenderer::unloadMeshes(ChunkManager& chunkManager)
{
	for (const ChunkCoord& coord : chunkManager.meshUnload)
		chunksMeshesByPosition.erase(coord);

	chunkManager.meshUnload.clear();
	meshListDirty = true;
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
		std::vector<Quad> quads;
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
		quads = buildMeshData(chunk.get(), negativeX.get(), positiveX.get(), negativeZ.get(), positiveZ.get(), chunkManager);
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
	meshListDirty = true;
}

std::vector<Quad> VoxelRenderer::buildMeshData(Chunk* chunk, ChunkManager& chunkManager)
{
	std::vector<Quad> chunkQuads;
	chunkQuads.reserve(512);

	const float offsetX = chunk->position.x * Chunk::SIZE_X;
	const float offsetZ = chunk->position.z * Chunk::SIZE_Z;

	int mask[MAX_SLICE];
	bool used[MAX_SLICE];

	for (uint8_t f = 0; f < 6; f++)
	{
		int normalAxis = (cubeFacesDirections[f][0] != 0) ? 0 : (cubeFacesDirections[f][1] != 0) ? 1 : 2;

		int uAxis = (normalAxis == 0) ? 1 : 0;
		int vAxis = (normalAxis == 2) ? 1 : 2;

		int sizeNormal = sizes[normalAxis];
		int sizeU = sizes[uAxis];
		int sizeV = sizes[vAxis];

		for (int n = 0; n < sizeNormal; n++)
		{
			for (int u = 0; u < sizeU; u++)
			for (int v = 0; v < sizeV; v++)
			{
				int coordinates[3];
				coordinates[normalAxis] = n;
				coordinates[uAxis] = u;
				coordinates[vAxis] = v;

				int blockX = coordinates[0];
				int blockY = coordinates[1];
				int blockZ = coordinates[2];

				BlockType blockType = chunk->blocks[blockX][blockZ][blockY];
				int neighbourChunks[3] = {
					blockX + cubeFacesDirections[f][0],
					blockY + cubeFacesDirections[f][1],
					blockZ + cubeFacesDirections[f][2]
				};

				bool exposed;
				if (blockType == B_WATER) {
					exposed = chunkManager.isAir(chunk, { neighbourChunks[0], neighbourChunks[1], neighbourChunks[2] });
				}
				else {
					exposed = (blockType != B_AIR)
						&& (chunkManager.isAir(chunk, { neighbourChunks[0], neighbourChunks[1], neighbourChunks[2] })
							|| chunkManager.isWater(chunk, { neighbourChunks[0], neighbourChunks[1], neighbourChunks[2] }));
				}

				int depth = chunk->heightMap[blockX][blockZ] - blockY;

				int skyLight = SHADE_LEVELS - 1 - (depth / SKYLIGHT_FALLOFF);

				if (skyLight < 0)
					skyLight = 0;

				if (skyLight > SHADE_LEVELS - 1)
					skyLight = SHADE_LEVELS - 1;

				mask[u * sizeV + v] = exposed ? ((int)blockType * SHADE_LEVELS + skyLight) : -1;
				used[u * sizeV + v] = false;
			}

			for (int u = 0; u < sizeU; u++)
			for (int v = 0; v < sizeV; v++)
			{
				int index = u * sizeV + v;
				if (used[index] || mask[index] < 0)
					continue;

				int cell = mask[index];
				int blockType = mask[index] / SHADE_LEVELS;
				int skyLight = mask[index] % SHADE_LEVELS;

				int spanV = 1;
				while (
					v + spanV < sizeV &&
					!used[u * sizeV + v + spanV] &&
					mask[u * sizeV + v + spanV] == cell
					)
					spanV++;

				int spanU = 1;
				while (u + spanU < sizeU)
				{
					bool ok = true;
					for (int k = 0; k < spanV && ok; k++)
					{
						int neighbourIndex = (u + spanU) * sizeV + v + k;
						if (used[neighbourIndex] || mask[neighbourIndex] != cell)
							ok = false;
					}
					if (!ok)
						break;
					spanU++;
				}

				for (int du = 0; du < spanU; du++)
				for (int dv = 0; dv < spanV; dv++)
					used[(u + du) * sizeV + v + dv] = true;

				uint8_t faceNormal = (n + (cubeFacesDirections[f][normalAxis] > 0 ? 1 : 0));

				Quad quad = {};

				quad.faceIndex = f;
				quad.normal = faceNormal;
				quad.uStart = u;
				quad.vStart = v;
				quad.uSpan = spanU;
				quad.vSpan = spanV;
				quad.color = chunkManager.BLOCK_PROPERTIES[blockType].faceColors[f];
				quad.skyLight = (uint8_t)skyLight;

				chunkQuads.push_back(quad);
			}
		}
	}

	return chunkQuads;
}

std::vector<Quad> VoxelRenderer::buildMeshData(Chunk* chunk, Chunk* negativeXNeighbour, Chunk* positiveXNeighbour, Chunk* negativeZNeighbour, Chunk* positiveZNeighbour, ChunkManager& chunkManager)
{
	std::vector<Quad> chunkQuads;
	chunkQuads.reserve(512);

	const float offsetX = chunk->position.x * Chunk::SIZE_X;
	const float offsetZ = chunk->position.z * Chunk::SIZE_Z;

	int mask[MAX_SLICE];
	bool used[MAX_SLICE];
	uint8_t light[MAX_SLICE];

	for (uint8_t f = 0; f < 6; f++)
	{
		int normalAxis = (cubeFacesDirections[f][0] != 0) ? 0 : (cubeFacesDirections[f][1] != 0) ? 1 : 2;
		int uAxis = (normalAxis == 0) ? 1 : 0;
		int vAxis = (normalAxis == 2) ? 1 : 2;

		int sizeNormal = sizes[normalAxis];
		int sizeU = sizes[uAxis];
		int sizeV = sizes[vAxis];

		for (int n = 0; n < sizeNormal; n++)
		{
			for (int u = 0; u < sizeU; u++)
			for (int v = 0; v < sizeV; v++)
			{
				int coordinates[3];
				coordinates[normalAxis] = n;
				coordinates[uAxis] = u;
				coordinates[vAxis] = v;

				int blockX = coordinates[0];
				int blockY = coordinates[1];
				int blockZ = coordinates[2];

				BlockType blockType = chunk->blocks[blockX][blockZ][blockY];
				int neighbourChunks[3] = {
					blockX + cubeFacesDirections[f][0],
					blockZ + cubeFacesDirections[f][2],
					blockY + cubeFacesDirections[f][1]
				};

				bool exposed;
				if (blockType == B_WATER) {
					exposed = chunkManager.isAir(chunk, negativeXNeighbour, positiveXNeighbour, negativeZNeighbour, positiveZNeighbour, { neighbourChunks[0], neighbourChunks[2], neighbourChunks[1] });
				}
				else {
					exposed = (blockType != B_AIR)
						&& (chunkManager.isAir(chunk, negativeXNeighbour, positiveXNeighbour, negativeZNeighbour, positiveZNeighbour, { neighbourChunks[0], neighbourChunks[2], neighbourChunks[1] })
							|| chunkManager.isWater(chunk, negativeXNeighbour, positiveXNeighbour, negativeZNeighbour, positiveZNeighbour, { neighbourChunks[0], neighbourChunks[2], neighbourChunks[1] }));
				}

				uint8_t neighbourLight = chunkManager.getNeighbourLight(chunk, negativeXNeighbour, positiveXNeighbour, negativeZNeighbour, positiveZNeighbour, { neighbourChunks[0], neighbourChunks[2], neighbourChunks[1] });
				neighbourLight = neighbourLight < 0 ? 0 : neighbourLight;
				mask[u * sizeV + v] = exposed ? ((int)blockType * SHADE_LEVELS + neighbourLight) : -1;
				used[u * sizeV + v] = false;
				light[u * sizeV + v] = neighbourLight;
			}

			for (int u = 0; u < sizeU; u++)
			for (int v = 0; v < sizeV; v++)
			{
				int index = u * sizeV + v;
				if (used[index] || mask[index] < 0)
					continue;

				int cell = mask[index];
				int blockType = cell / SHADE_LEVELS;
				int skyLight = cell % SHADE_LEVELS;

				int spanV = 1;
				while (
					v + spanV < sizeV &&
					!used[u * sizeV + v + spanV] &&
					mask[u * sizeV + v + spanV] == cell
					)
					spanV++;

				int spanU = 1;
				while (u + spanU < sizeU)
				{
					bool ok = true;
					for (int k = 0; k < spanV && ok; k++)
					{
						int neighbourIndex = (u + spanU) * sizeV + v + k;
						if (used[neighbourIndex] || mask[neighbourIndex] != cell)
							ok = false;
					}
					if (!ok)
						break;
					spanU++;
				}

				for (int du = 0; du < spanU; du++)
				for (int dv = 0; dv < spanV; dv++)
					used[(u + du) * sizeV + v + dv] = true;

				uint8_t faceNormal = (n + (cubeFacesDirections[f][normalAxis] > 0 ? 1 : 0));

				Quad quad = {};

				quad.faceIndex = f;
				quad.normal = faceNormal;
				quad.uStart = u;
				quad.vStart = v;
				quad.uSpan = spanU;
				quad.vSpan = spanV;
				quad.color = chunkManager.BLOCK_PROPERTIES[blockType].faceColors[f];
				quad.skyLight = light[u * sizeV + v];

				chunkQuads.push_back(quad);
			}
		}
	}

	return chunkQuads;
}

Vector3 VoxelRenderer::makeVertex(float fu, float fv, int normalAxis, float faceNormal, int uAxis, int vAxis, const float offsetX, const float offsetZ)
{
	float coords[3] = { 0.0f, 0.0f, 0.0f };
	coords[normalAxis] = faceNormal;
	coords[uAxis] = fu;
	coords[vAxis] = fv;
	return { coords[0] + offsetX, coords[1], coords[2] + offsetZ };
}

Vector3 VoxelRenderer::reconstructCenter(const Quad& quad, float offsetX, float offsetZ, float normalAxis, float uAxis, float vAxis)
{
	Vector3Int direction = cubeFacesDirections[quad.faceIndex];
	
	float uCenter = quad.uStart + quad.uSpan * 0.5f;
	float vCenter = quad.vStart + quad.vSpan * 0.5f;

	return makeVertex(uCenter, vCenter, normalAxis, (float)quad.normal, uAxis, vAxis, offsetX, offsetZ);
}

void VoxelRenderer::reconstructQuad(const Quad& quad, float offsetX, float offsetZ, float normalAxis, float uAxis, float vAxis, Vector3& v0, Vector3& v1, Vector3& v2, Vector3& v3)
{
	float faceNormal = (float)quad.normal;

	float uStart = (float)quad.uStart;
	float vStart = (float)quad.vStart;

	float uStartSpan = (float)(quad.uStart + quad.uSpan);
	float vStartSpan = (float)(quad.vStart + quad.vSpan);

	v0 = makeVertex(uStart, vStart, normalAxis, faceNormal, uAxis, vAxis, offsetX, offsetZ);
	v1 = makeVertex(uStartSpan, vStart, normalAxis, faceNormal, uAxis, vAxis, offsetX, offsetZ);
	v2 = makeVertex(uStartSpan, vStartSpan, normalAxis, faceNormal, uAxis, vAxis, offsetX, offsetZ);
	v3 = makeVertex(uStart, vStartSpan, normalAxis, faceNormal, uAxis, vAxis, offsetX, offsetZ);
}
