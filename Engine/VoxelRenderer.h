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

class VoxelRenderer
{
public:
	void render(Renderer& renderer, Camera& camera);
	std::unordered_map<ChunkCoord, std::vector<Quad>, ChunkCoordHash> chunksMeshesByPosition;

	void meshBuilderWorker(ChunkManager& chunkManager);
	void unloadMeshes(ChunkManager& chunkManager);
	void commitReadyMeshes();

private:
	Vector3 convertToCameraSpace(Vector3& position, Camera& camera);
	Vertex projectViewSpacePoint(Vector3& position, Camera& camera, float aspectRatio, int width, int height);
	int clipNearPlane(Vector3* source, int vertexCount, Vector3* output, Camera& camera);

	std::queue<ReadyMesh> readyMeshes;
	std::mutex readyMeshesMutex;

	std::vector<Quad> buildMeshData(Chunk* chunk, ChunkManager& chunkManager);

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

	const int faceIndices[6][4] =
	{
		{0,1,2,3},
		{5,4,7,6},

		{4,0,3,7},
		{1,5,6,2},

		{3,2,6,7},
		{4,5,1,0}
	};

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
