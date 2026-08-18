#pragma once
#include <unordered_map>
#include "DirectionsXZ.h"
#include "ReadyMesh.h"
#include "Quad.h"
#include "Frustum.h"
#include "VisibleMesh.h"
#include "../Core/Vector.h"
#include "../Terminal/Renderer.h"
#include "../Core/Vertex.h"
#include "../World/Camera.h"
#include "../World/Chunk/Chunk.h"
#include "../World/ChunkManager.h"
#include "../World/Cube.h"
#include "../World/Lighting.h"

class VoxelRenderer
{
public:
	void render(Renderer& renderer, Camera& camera, Lighting& lighting);
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

	std::vector<VisibleMesh> meshDrawList;
	bool meshListDirty = true;

	std::vector<Quad> buildMeshData(Chunk* chunk, ChunkManager& chunkManager);
	std::vector<Quad> buildMeshData(Chunk* chunk, Chunk* negativeXNeighbour, Chunk* positiveXNeighbour, Chunk* negativeZNeighbour, Chunk* positiveZNeighbour, ChunkManager& chunkManager);

	Vector3 makeVertex(float fu, float fv, int normalAxis, float faceNormal, int uAxis, int vAxis, const float offsetX, const float offsetZ);
	Vector3 reconstructCenter(const Quad& quad, float offsetX, float offsetZ, float normalAxis, float uAxis, float vAxis);

	void reconstructQuad(const Quad& quad, float offsetX, float offsetZ, float normalAxis, float uAxis, float vAxis, Vector3& v0, Vector3& v1, Vector3& v2, Vector3& v3);

	const float FRUSTUM_PADDING = 0.087f;

	const int sizes[3] = { Chunk::SIZE_X , Chunk::SIZE_Y, Chunk::SIZE_Z };

	const int normalAxisTable[6][3] = {
		{2,0,1}, {2,0,1},
		{0,1,2}, {0,1,2},
		{1,0,2}, {1,0,2},
	};
	
	static const int MAX_SLICE = Chunk::SIZE_X * Chunk::SIZE_Y;

	const DirectionsXZ neighboursDirections[4] = {
		{ 1,  0},
		{-1,  0},
		{ 0,  1},
		{ 0, -1}
	};
};
