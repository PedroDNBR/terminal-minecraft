#pragma once
#include <unordered_map>
#include "DirectionsXZ.h"
#include "ReadyMesh.h"
#include "../Renderer/Vector.h"
#include "../Renderer/Renderer.h"
#include "../Renderer/Vertex.h"
#include "../Renderer/Quad.h"
#include "../World/Camera.h"
#include "../World/Chunk/Chunk.h"
#include "../World/ChunkManager.h"
#include "Frustum.h"

class VoxelRenderer
{
public:
	void render(Renderer& renderer, Camera& camera);
	std::unordered_map<ChunkCoord, std::vector<Quad>, ChunkCoordHash> chunksMeshesByPosition;

	void meshBuilderWorker(ChunkManager& chunkManager);
	void unloadMeshes(ChunkManager& chunkManager);
	void commitReadyMeshes();

private:
	Vector3 convertToCameraSpace(const Vector3& position, Camera& camera);
	Vertex projectViewSpacePoint(const Vector3& position, Camera& camera, float aspectRatio, int width, int height);
	int clipNearPlane(Vector3* source, int vertexCount, Vector3* output, Camera& camera);

	Frustum buildFrustum(const Camera& camera, float aspectRatio, float fovRadius);
	Plane makePlane(Vector3 normal, const Camera& camera);

	std::queue<ReadyMesh> readyMeshes;
	std::mutex readyMeshesMutex;

	std::vector<Quad> buildMeshData(Chunk* chunk, ChunkManager& chunkManager);

	Vector3 makeVertex(float fu, float fv, int normalAxis, float faceNormal, int uAxis, int vAxis, const float offsetX, const float offsetZ);

	const float FRUSTUM_PADDING = 0.087f;

	const int sizes[3] = { Chunk::SIZE_X , Chunk::SIZE_Y, Chunk::SIZE_Z };
	
	static const int MAX_SLICE = Chunk::SIZE_X * Chunk::SIZE_Y;

	const Vector3Int cubeFacesDirections[6] = {
		{0,0,-1},
		{0,0,1},
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
