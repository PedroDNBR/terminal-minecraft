#include "TerrainGenerator.h"
#include "Noise.h"

std::unique_ptr<Chunk> TerrainGenerator::generateChunkData(ChunkCoord chunkPosition)
{
	std::unique_ptr<Chunk> chunk = std::make_unique<Chunk>();
	chunk->position = chunkPosition;
	std::memset(chunk->blocks, AIR, sizeof(chunk->blocks));

	NoiseCache noiseCache = buildNoiseCache(chunkPosition, seed);

	int heightMap[Chunk::SIZE_X][Chunk::SIZE_Z];
	bool isRockyMap[Chunk::SIZE_X][Chunk::SIZE_Z];
	bool isRiverMap[Chunk::SIZE_X][Chunk::SIZE_Z];
	int riverLevelMap[Chunk::SIZE_X][Chunk::SIZE_Z];

	const int worldOffsetX = chunkPosition.x * Chunk::SIZE_X;
	const int worldOffsetZ = chunkPosition.z * Chunk::SIZE_Z;

	for (int x = 0; x < Chunk::SIZE_X; x++)
	for (int z = 0; z < Chunk::SIZE_Z; z++)
	{
		float gridX = (float)x / NoiseConstants::NOISE_STEP;
		float gridZ = (float)z / NoiseConstants::NOISE_STEP;
		int intX = (int)gridX;
		int intZ = (int)gridZ;
		float fractionX = gridX - intX;
		float fractionZ = gridZ - intZ;

		float continent = bilinearLerp(noiseCache.base, intX, intZ, fractionX, fractionZ);
		float erosion = bilinearLerp(noiseCache.hills, intX, intZ, fractionX, fractionZ);
		float peaks = bilinearLerp(noiseCache.peaks, intX, intZ, fractionX, fractionZ);
		peaks = PEAK_SHAPE_MAX - fabsf(peaks * PEAK_SHAPE_SCALE - PEAK_SHAPE_OFFSET);
		peaks = peaks * peaks * peaks;

		float erosionMask = EROSION_MASK_MAX - smoothstep(erosion, EROSION_START, EROSION_END);

		float valleys = bilinearLerp(noiseCache.valleys, intX, intZ, fractionX, fractionZ);

		float valleyEffect = (VALLEY_MIDPOINT - valleys) * VALLEY_STRENGTH;
		if (valleyEffect < 0) valleyEffect *= NEGATIVE_VALLEY_MULTIPLIER;

		float heightFinal = BASE_TERRAIN_HEIGHT +
			continent * CONTINENT_ELEVATION_MAX +
			peaks * EROSION_PEAKS_MAX * erosionMask +
			(EROSION_MASK_MAX - erosionMask) * PEAK_SHAPE_SCALE +
			valleyEffect * VALLEY_SCALE;

		heightFinal += OFFSET_ROUND;

		if(heightFinal < 1)
			heightFinal = 1;
		
		if(heightFinal > Chunk::SIZE_Y - 1)
			heightFinal = Chunk::SIZE_Y - 1;

		heightMap[x][z] = (int)heightFinal + OFFSET_ROUND;
		isRockyMap[x][z] = (heightMap[x][z] > 48);
		isRiverMap[x][z] = false;
		riverLevelMap[x][z] = 0;
	}

	for (int x = 0; x < Chunk::SIZE_X; x++)
	for (int z = 0; z < Chunk::SIZE_Z; z++)
	{
		int height = heightMap[x][z];
		int heightX = heightMap[x < Chunk::SIZE_X - 1 ? x + 1 : x][z];
		int heightZ = heightMap[x][z < Chunk::SIZE_Z - 1 ? z + 1 : z];

		float steepness = fabsf((float)(height - heightX)) + fabsf((float)(height - heightZ));
		isRockyMap[x][z] = (height > 58) || (height > 50 && steepness > .6f);
		
		float gridX = (float)x / NoiseConstants::NOISE_STEP;
		float gridZ = (float)z / NoiseConstants::NOISE_STEP;

		int intX = (int)gridX;
		int intZ = (int)gridZ;

		float fractionX = gridX - intX;
		float fractionZ = gridZ - intZ;

		float riverX = bilinearLerp(noiseCache.riverX, intX, intZ, fractionX, fractionZ);
		
		float riverZ = bilinearLerp(noiseCache.riverZ, intX, intZ, fractionX, fractionZ);

		float riverDistance = fabsf(riverX - 0.5f) + fabsf(riverZ - 0.5f);

		if (
			riverDistance < RIVER_WIDTH &&
			steepness < RIVER_MAX_STEEP &&
			!isRockyMap[x][z] &&
			height > SEA_LEVEL
			)
		{
			int bedDepth = 5 + (int)(riverDistance * 8.0f);
			int newHeight = height - bedDepth;
			if (newHeight < 1) 
				newHeight = 1;

			riverLevelMap[x][z] = newHeight + 1;
			heightMap[x][z] = newHeight;
			isRiverMap[x][z] = true;
			isRockyMap[x][z] = false;
		}
	}

	float caveGrid[NoiseConstants::CAVE_GRID_X][NoiseConstants::CAVE_GRID_Y][NoiseConstants::CAVE_GRID_Z];

	for (int x = 0; x < NoiseConstants::CAVE_GRID_X; x++)
	for (int y = 0; y < NoiseConstants::CAVE_GRID_Y; y++)
	for (int z = 0; z < NoiseConstants::CAVE_GRID_Z; z++)
	{
		int worldX = worldOffsetX + x * NoiseConstants::CAVE_STEP;
		int worldY = y * NoiseConstants::CAVE_STEP;
		int worldZ = worldOffsetZ + z * NoiseConstants::CAVE_STEP;

		float n1 = valueNoise3D(worldX, worldY, worldZ, int(seed * .5f));
		float n2 = valueNoise3D(worldX + 47, worldY + 31, worldZ + 83, seed * 2);
		caveGrid[x][y][z] = fabsf(n1 - 0.5f) + fabsf(n2 - 0.5f);
	}

	for (int x = 0; x < Chunk::SIZE_X; x++)
	for (int z = 0; z < Chunk::SIZE_Z; z++)
	{
		int height = heightMap[x][z];
		bool isRocky = isRockyMap[x][z];
		bool isRiver = isRiverMap[x][z];

		for (int y = 0; y <= height; y++)
		{
			BlockType blockType;
			if (y == height)
				blockType = isRocky ? STONE : GRASS;
			else if (y < height && y > height - 4)
				blockType = DIRT;
			else if (y == 0)
				blockType = BED_ROCK;
			else
				blockType = STONE;

			chunk->blocks[x][y][z] = blockType;
		}

		if (height < SEA_LEVEL)
		{
			for (int y = height + 1; y <= SEA_LEVEL; y++)
				chunk->blocks[x][y][z] = WATER;

			chunk->blocks[x][height][z] = SAND;
			if (height > 0)
				chunk->blocks[x][height - 1][z] = SAND;
		}

		if (
			height >= SEA_LEVEL && height <= SEA_LEVEL + 1 &&
			chunk->blocks[x][height][z] == GRASS
		)
			chunk->blocks[x][height][z] = SAND;

		if (isRiver)
		{
			int riverLevel = riverLevelMap[x][z];
			for (int y = height + 1; y <= riverLevel; y++)
				chunk->blocks[x][y][z] = WATER;
		}

		int caveTop = height - 2;
		if (caveTop <= 1) continue;

		for (int y = 1; y < caveTop; y++)
		{
			if (chunk->blocks[x][y][z] == WATER) continue;

			float caveGridX = (float)x / NoiseConstants::CAVE_STEP;
			float caveGridY = (float)y / (NoiseConstants::CAVE_STEP * .6f);
			float caveGridZ = (float)z / NoiseConstants::CAVE_STEP;

			int intGridX = (int)caveGridX;
			int intGridY = (int)caveGridY;
			int intGridZ = (int)caveGridZ;

			float fractionGridX = caveGridX - intGridX;
			float fractionGridY = caveGridY - intGridY;
			float fractionGridZ = caveGridZ - intGridZ;

			if (intGridX >= NoiseConstants::CAVE_GRID_X - 1)
				intGridX = NoiseConstants::CAVE_GRID_X - 2;

			if (intGridY >= NoiseConstants::CAVE_GRID_Y - 1)
				intGridY = NoiseConstants::CAVE_GRID_Y - 2;

			if (intGridZ >= NoiseConstants::CAVE_GRID_Z - 1)
				intGridZ = NoiseConstants::CAVE_GRID_Z - 2;

			float c000 = caveGrid[intGridX][intGridY][intGridZ];
			float c100 = caveGrid[intGridX + 1][intGridY][intGridZ];
			float c010 = caveGrid[intGridX][intGridY + 1][intGridZ];
			float c110 = caveGrid[intGridX + 1][intGridY + 1][intGridZ];
			float c001 = caveGrid[intGridX][intGridY][intGridZ + 1];
			float c101 = caveGrid[intGridX + 1][intGridY][intGridZ + 1];
			float c011 = caveGrid[intGridX][intGridY + 1][intGridZ + 1];
			float c111 = caveGrid[intGridX + 1][intGridY + 1][intGridZ + 1];

			float cx0 = c000 * (1 - fractionGridX) + c100 * fractionGridX;
			float cx1 = c010 * (1 - fractionGridX) + c110 * fractionGridX;
			float cx2 = c001 * (1 - fractionGridX) + c101 * fractionGridX;
			float cx3 = c011 * (1 - fractionGridX) + c111 * fractionGridX;

			float cz0 = cx0 * (1 - fractionGridZ) + cx2 * fractionGridZ;
			float cz1 = cx1 * (1 - fractionGridZ) + cx3 * fractionGridZ;

			float caveValue = cz0 * (1 - fractionGridY) + cz1 * fractionGridY;
		
			if(caveValue < CAVE_THRESHOUD)
				chunk->blocks[x][y][z] = AIR;
		}
	}

	for (int x = 0; x < Chunk::SIZE_X; x++)
	for (int z = 0; z < Chunk::SIZE_Z; z++)
	{
		if (
			isRockyMap[x][z] ||
			isRiverMap[x][z] ||
			chunk->blocks[x][heightMap[x][z]][z] == SAND
			)
			continue;

		int surfaceY = heightMap[x][z];

		if (surfaceY <= SEA_LEVEL)
			continue;

		float gridX = (float)x / NoiseConstants::NOISE_STEP;
		float gridZ = (float)z / NoiseConstants::NOISE_STEP;
		int intX = (int)gridX;
		int intZ = (int)gridZ;
		float fractionX = gridX - intX;
		float fractionZ = gridZ - intZ;

		float forestDensity = bilinearLerp(noiseCache.forest, intX, intZ, fractionX, fractionZ);

		if (forestDensity < MIN_FOREST_DENSITY)
			continue;

		unsigned hash = (unsigned)(
			(unsigned)(worldOffsetX + x) * 73856093u ^
			(unsigned)(worldOffsetZ + z) * 19349663u ^
			seed
		);

		if ((hash % FOREST_TREE_PLACEMENT_OFFSET) != 0)
			continue;

		placeTree(chunk.get(), x, surfaceY, z);

	}

	return chunk;
}

NoiseCache TerrainGenerator::buildNoiseCache(ChunkCoord chunkPosition, uint32_t seed)
{
	NoiseCache noiseCache;

	for (int gx = 0; gx < NoiseConstants::NOISE_GRID; gx++)
	for (int gz = 0; gz < NoiseConstants::NOISE_GRID; gz++)
	{
		float worldX = (float)(chunkPosition.x * Chunk::SIZE_X + gx * NoiseConstants::NOISE_GRID);
		float worldZ = (float)(chunkPosition.z * Chunk::SIZE_Z + gz * NoiseConstants::NOISE_GRID);

		noiseCache.base[gx][gz] = smoothNoise(worldX * NoiseConstants::BASE_FREQUENCY, worldZ * NoiseConstants::BASE_FREQUENCY, seed + 1u);
		noiseCache.hills[gx][gz] = fractalNoise(worldX * NoiseConstants::BASE_FREQUENCY, worldZ * NoiseConstants::BASE_FREQUENCY, seed + 2u);
		noiseCache.peaks[gx][gz] = smoothNoise(worldX * NoiseConstants::BASE_FREQUENCY, worldZ * NoiseConstants::BASE_FREQUENCY, seed + 3u);
		noiseCache.valleys[gx][gz] = smoothNoise(worldX * NoiseConstants::BASE_FREQUENCY, worldZ * NoiseConstants::BASE_FREQUENCY, seed + 777u);
		noiseCache.forest[gx][gz] = smoothNoise(worldX * NoiseConstants::BASE_FREQUENCY, worldZ * NoiseConstants::BASE_FREQUENCY, seed + 5555u);
		noiseCache.riverX[gx][gz] = smoothNoise(worldX * NoiseConstants::BASE_FREQUENCY, worldZ * NoiseConstants::BASE_FREQUENCY, seed + 7777u);
		noiseCache.riverZ[gx][gz] = smoothNoise(worldX * NoiseConstants::BASE_FREQUENCY, worldZ * NoiseConstants::BASE_FREQUENCY, seed + 8888u);
	}

	return noiseCache;
}

void TerrainGenerator::placeTree(Chunk* chunk, int x, int baseY, int z)
{
	unsigned hash = (unsigned)(x * 1619 + z * 31337 + baseY * 997);

	int trunkHeight = TREE_TRUNK_BASE_HEIGHT + (int)(hash % 3);

	for (int y = 1; y <= trunkHeight; y++)
	{
		int trunkY = baseY + y;
		if (trunkY >= Chunk::SIZE_Y)
			break;

		chunk->blocks[x][trunkY][z] = LOG;
	}

	int topY = baseY + trunkHeight;

	for (int deltaY = -1; deltaY <= TREE_TRUNK_LEAVES_Y_RADIUS; deltaY++)
	for (int deltaX = -TREE_TRUNK_LEAVES_XZ_RADIUS; deltaX <= TREE_TRUNK_LEAVES_XZ_RADIUS; deltaX++)
	for (int deltaZ = -TREE_TRUNK_LEAVES_XZ_RADIUS; deltaZ <= TREE_TRUNK_LEAVES_XZ_RADIUS; deltaZ++)
	{
		float elipseX = (float)deltaX / TREE_TRUNK_LEAVES_XZ_RADIUS;
		float elipseY = (float)deltaY / TREE_TRUNK_LEAVES_Y_RADIUS;
		float elipseZ = (float)deltaZ / TREE_TRUNK_LEAVES_XZ_RADIUS;

		if (elipseX * elipseX + elipseY * elipseY + elipseZ * elipseZ > 1.0f)
			continue;	

		int targetX = x + deltaX;
		int targetY = topY + deltaY;
		int targetZ = z + deltaZ;

		if (targetX < 0 || targetX >= Chunk::SIZE_X)
			continue;
		if (targetY < 0 || targetY >= Chunk::SIZE_Y)
			continue;
		if (targetZ < 0 || targetZ >= Chunk::SIZE_Z)
			continue;

		if (chunk->blocks[targetX][targetY][targetZ] == LOG)
			continue;

		chunk->blocks[targetX][targetY][targetZ] = LEAVES;
	}
}

float TerrainGenerator::bilinearLerp(float grid[NoiseConstants::NOISE_GRID][NoiseConstants::NOISE_GRID],
	int intX, int intZ, float fractionX, float fractionZ)
{
	return grid[intX][intZ] * (1 - fractionX) * (1 - fractionZ) + 
		grid[intX + 1][intZ] * fractionX * (1 - fractionZ) + 
		grid[intX][intZ + 1] * (1 - fractionX) * fractionZ + 
		grid[intX + 1][intZ + 1] * fractionX * fractionZ;
}
