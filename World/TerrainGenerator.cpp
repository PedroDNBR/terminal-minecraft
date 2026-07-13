#include "TerrainGenerator.h"
#include "Noise/Noise.h"

std::unique_ptr<Chunk> TerrainGenerator::generateChunkData(ChunkCoord chunkPosition)
{
	std::unique_ptr<Chunk> chunk = std::make_unique<Chunk>();
	chunk->position = chunkPosition;
	std::memset(chunk->blocks, AIR, sizeof(chunk->blocks));

	NoiseCache noiseCache = buildNoiseCache(chunkPosition, seed);

	int heightMap[Chunk::SIZE_X][Chunk::SIZE_Z];
	bool isRockyMap[Chunk::SIZE_X][Chunk::SIZE_Z];

	float temperatureMap[Chunk::SIZE_X][Chunk::SIZE_Z];
	float humidityMap[Chunk::SIZE_X][Chunk::SIZE_Z];

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

		temperatureMap[x][z] = bilinearLerp(noiseCache.temperature, intX, intZ, fractionX, fractionZ);
		humidityMap[x][z] = bilinearLerp(noiseCache.humidity, intX, intZ, fractionX, fractionZ);

		float continent = bilinearLerp(noiseCache.base, intX, intZ, fractionX, fractionZ);
		float erosion = bilinearLerp(noiseCache.hills, intX, intZ, fractionX, fractionZ);
		float peaks = bilinearLerp(noiseCache.peaks, intX, intZ, fractionX, fractionZ);
		peaks = PEAK_SHAPE_MAX - fabsf(peaks * PEAK_SHAPE_SCALE - PEAK_SHAPE_OFFSET);
		peaks = peaks * peaks;

		float erosionMask = EROSION_MASK_MAX - smoothstep(erosion, EROSION_START, EROSION_END);

		float valleys = bilinearLerp(noiseCache.valleys, intX, intZ, fractionX, fractionZ);

		float valleyEffect = (VALLEY_MIDPOINT - valleys) * VALLEY_STRENGTH;
		if (valleyEffect < 0) valleyEffect *= NEGATIVE_VALLEY_MULTIPLIER;

		float temperature = temperatureMap[x][z];

		float relief = peaks * EROSION_PEAKS_MAX * erosionMask
			+ (EROSION_MASK_MAX - erosionMask) * PEAK_SHAPE_SCALE
			+ valleyEffect * VALLEY_SCALE;

		float reliefFactor = 1.0f - temperature * DESERT_FLATTEN;

		float biomeElevation = (temperature < 0.5f) ? (0.5f - temperature) * BIOME_ELEVATION : 0.0f;

		float heightFinal = BASE_TERRAIN_HEIGHT
			+ continent * CONTINENT_ELEVATION_MAX
			+ relief * reliefFactor
			+ biomeElevation;

		heightFinal += OFFSET_ROUND;

		if(heightFinal < 1)
			heightFinal = 1;
		
		if(heightFinal > Chunk::SIZE_Y - 1)
			heightFinal = Chunk::SIZE_Y - 1;

		heightMap[x][z] = (int)heightFinal + OFFSET_ROUND;
		isRockyMap[x][z] = (heightMap[x][z] > 48);
	}

	for (int x = 0; x < Chunk::SIZE_X; x++)
	for (int z = 0; z < Chunk::SIZE_Z; z++)
	{
		int height = heightMap[x][z];
		int heightX = heightMap[x < Chunk::SIZE_X - 1 ? x + 1 : x][z];
		int heightZ = heightMap[x][z < Chunk::SIZE_Z - 1 ? z + 1 : z];

		float steepness = fabsf((float)(height - heightX)) + fabsf((float)(height - heightZ));
		isRockyMap[x][z] = (height > 58) || (height > 50 && steepness > .6f);
	}

	float caveGrid[NoiseConstants::CAVE_GRID_X][NoiseConstants::CAVE_GRID_Z][NoiseConstants::CAVE_GRID_Y];

	for (int x = 0; x < NoiseConstants::CAVE_GRID_X; x++)
	for (int z = 0; z < NoiseConstants::CAVE_GRID_Z; z++)
	for (int y = 0; y < NoiseConstants::CAVE_GRID_Y; y++)
	{
		int worldX = worldOffsetX + x * NoiseConstants::CAVE_STEP;
		int worldY = y * NoiseConstants::CAVE_STEP;
		int worldZ = worldOffsetZ + z * NoiseConstants::CAVE_STEP;

		float n1 = valueNoise3D(worldX, worldY, worldZ, int(seed * .5f));
		float n2 = valueNoise3D(worldX + 47, worldY + 31, worldZ + 83, seed * 2);
		caveGrid[x][z][y] = fabsf(n1 - 0.5f) + fabsf(n2 - 0.5f);
	}

	for (int x = 0; x < Chunk::SIZE_X; x++)
		for (int z = 0; z < Chunk::SIZE_Z; z++)
		{
			int height = heightMap[x][z];
			bool isRocky = isRockyMap[x][z];
			float temperature = temperatureMap[x][z];
			float humidity = humidityMap[x][z];

			for (int y = 0; y <= height; y++)
			{
				BlockType blockType;
				if (y == height)
				{
					if (temperature > 0.55f && humidity < 0.4f)
						blockType = SAND;
					else if (temperature < 0.3f && humidity < 0.4f)
						blockType = STONE;
					else if (humidity > 0.6f && !isRocky)
						blockType = GRASS;
					else
						blockType = GRASS;
				}
				else if (y < height && y > height - 4)
					if (temperature > 0.55f && humidity < 0.4f)
						blockType = SAND;
					else if (temperature < 0.3f && humidity < 0.4f)
						blockType = STONE;
					else
						blockType = DIRT;

				else if (y == 0)
					blockType = BED_ROCK;
				else
					blockType = STONE;

				chunk->blocks[x][z][y] = blockType;
			}

			if (height < SEA_LEVEL)
			{
				bool dryBiome = temperature > 0.55f && humidity < 0.4f;
				if (!dryBiome)
				{
					for (int y = height + 1; y <= SEA_LEVEL; y++)
						chunk->blocks[x][z][y] = WATER;
					chunk->blocks[x][z][height] = SAND;
					if (height > 0) chunk->blocks[x][z][height - 1] = SAND;
				}
			}

			if (
				height >= SEA_LEVEL && height <= SEA_LEVEL + 1 &&
				chunk->blocks[x][z][height] == GRASS
				)
				chunk->blocks[x][z][height] = SAND;

			int caveTop = height - 2;
			if (caveTop <= 1) continue;

		}

	for (int x = 0; x < Chunk::SIZE_X; x++)
		for (int z = 0; z < Chunk::SIZE_Z; z++)
		{
			if (
				isRockyMap[x][z] ||
				chunk->blocks[x][z][heightMap[x][z]] == STONE
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

		bool isDesert = (temperatureMap[x][z] > 0.55f && humidityMap[x][z] < 0.4f);

		unsigned hash = (unsigned)(
			(unsigned)(worldOffsetX + x) * 73856093u ^
			(unsigned)(worldOffsetZ + z) * 19349663u ^
			seed
		);

		if (isDesert)
		{
			if (chunk->blocks[x][z][surfaceY] != SAND)
				continue;

			if (forestDensity < .70f)
				continue;

			if ((hash % 350) != 0)
				continue;

			placeCactus(chunk.get(), x, surfaceY, z);
		}
		else
		{
			if (chunk->blocks[x][z][surfaceY] != GRASS)
				continue;

			if (forestDensity < MIN_FOREST_DENSITY)
				continue;

			if ((hash % FOREST_TREE_PLACEMENT_OFFSET) != 0)
				continue;

			placeTree(chunk.get(), x, surfaceY, z);
		}
	}

	unsigned wormHash = (unsigned)(worldOffsetX) * 73856093u ^ 
		(unsigned)(worldOffsetZ) * 19349663u ^ 
		((unsigned)seed * 2654435761u);

	wormHash = wormHash >> 15;
	wormHash *= 2246822519u;
	wormHash ^= wormHash >> 13;

	int wormCount = (wormHash % WORM_CAVE_FREQUENCY);

	for (int w = 0; w < wormCount; w++)
	{
		unsigned h = (wormHash + w * 0x9E3779B9u) * 2654435761u;
		h ^= h >> 16;

		unsigned bitsStartX = h & 0xF;
		unsigned bitsStartZ = (h >> 4) & 0xF;
		unsigned bitsY = (h >> 8) & 0x3F;
		unsigned bitsAngle = (h >> 14) & 0x3FF; 
		unsigned bitsDirectionY = (h >> 24) & 0x7F;

		int sx = (int)bitsStartX;
		int sz = (int)bitsStartZ;

		int columnHeight = heightMap[sx][sz];
		if (columnHeight < 8) continue;

		float startX = (float)sx;
		float startZ = (float)sz;
		float startY = (float)(4 + bitsY % (columnHeight - 6));

		float angle = (float)bitsAngle / 1024.0f * 6.2831853f;

		float directionX = cosf(angle);
		float directionZ = sinf(angle);
		float directionY = -0.4f + (float)bitsDirectionY / 128.0f * 0.8f;

		carveWormCave(chunk.get(), heightMap, startX, startY, startZ, directionX, directionY, directionZ, WORM_CAVE_RADIUS, WORM_CAVE_LENGTH);
	}

	return chunk;
}

NoiseCache TerrainGenerator::buildNoiseCache(ChunkCoord chunkPosition, uint32_t seed)
{
	NoiseCache noiseCache;

	for (int gx = 0; gx < NoiseConstants::NOISE_GRID; gx++)
	for (int gz = 0; gz < NoiseConstants::NOISE_GRID; gz++)
	{
		float worldX = (float)(chunkPosition.x * Chunk::SIZE_X + gx * NoiseConstants::NOISE_STEP);
		float worldZ = (float)(chunkPosition.z * Chunk::SIZE_Z + gz * NoiseConstants::NOISE_STEP);

		float warpedX = smoothNoise(worldX * NoiseConstants::WARP_FREQUENCY, worldZ * NoiseConstants::WARP_FREQUENCY, seed + 11111u);
		float warpedZ = smoothNoise(worldX * NoiseConstants::WARP_FREQUENCY, worldZ * NoiseConstants::WARP_FREQUENCY, seed + 22222u);

		noiseCache.temperature[gx][gz] = smoothNoise(worldX * NoiseConstants::BIOME_FREQUENCY, worldZ * NoiseConstants::BIOME_FREQUENCY, seed + 99991u);
		noiseCache.humidity[gx][gz] = smoothNoise(worldX * NoiseConstants::BIOME_FREQUENCY, worldZ * NoiseConstants::BIOME_FREQUENCY, seed + 88881u);

		noiseCache.base[gx][gz] = smoothNoise((worldX + warpedX * 30.0f) * NoiseConstants::BASE_FREQUENCY, (worldZ + warpedZ * 30.0f) * NoiseConstants::BASE_FREQUENCY, seed + 1u);
		noiseCache.hills[gx][gz] = fractalNoise(worldX * NoiseConstants::HILLS_FREQUENCY, worldZ * NoiseConstants::HILLS_FREQUENCY, seed + 2u);
		noiseCache.peaks[gx][gz] = smoothNoise(worldX * NoiseConstants::PEAKS_FREQUENCY, worldZ * NoiseConstants::PEAKS_FREQUENCY, seed + 3u);
		noiseCache.valleys[gx][gz] = smoothNoise(worldX * NoiseConstants::VALLEYS_FREQUENCY, worldZ * NoiseConstants::VALLEYS_FREQUENCY, seed + 777u);
		noiseCache.forest[gx][gz] = smoothNoise(worldX * NoiseConstants::FOREST_FREQUENCY, worldZ * NoiseConstants::FOREST_FREQUENCY, seed + 5555u);
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

		chunk->blocks[x][z][trunkY] = LOG;
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

		if (chunk->blocks[targetX][targetZ][targetY] == LOG)
			continue;

		chunk->blocks[targetX][targetZ][targetY] = LEAVES;
	}
}

void TerrainGenerator::placeCactus(Chunk* chunk, int x, int baseY, int z)
{
	unsigned hash = (unsigned)(x * 1619 + z * 31337 + baseY * 997);

	int trunkHeight = CACTUS_TRUNK_BASE_HEIGHT + (int)(hash % 3);

	for (int y = 1; y <= trunkHeight; y++)
	{
		int trunkY = baseY + y;
		if (trunkY >= Chunk::SIZE_Y)
			break;

		chunk->blocks[x][z][trunkY] = CACTUS;
	}
}

void TerrainGenerator::carveWormCave(Chunk* chunk, int heightMap[Chunk::SIZE_X][Chunk::SIZE_Z], float startX, float startY, float startZ, float directionX, float directionY, float directionZ, float radius, int length)
{
	for (int step = 0; step < length; step++)
	{
		float t = (float)step / length;
		directionX += smoothNoise(startX * 0.1f + t, startZ * 0.1f, seed + 1) * 0.4f - 0.2f;
		directionY += smoothNoise(startX * 0.1f, startZ * 0.1f + t, seed + 2) * 0.2f - 0.1f;
		directionZ += smoothNoise(startZ * 0.1f + t, startY * 0.1f, seed + 3) * 0.4f - 0.2f;

		float newWormLenght = sqrtf(directionX * directionX + directionY * directionY + directionZ * directionZ);

		if (newWormLenght < 1e-5f)
			newWormLenght = 1.0f;

		directionX /= newWormLenght;
		directionY /= newWormLenght;
		directionZ /= newWormLenght;

		startX += directionX;
		startY += directionY;
		startZ += directionZ;

		int caveX = (int)startX;
		int caveY = (int)startY;
		int caveZ = (int)startZ;
		int r = (int)radius;

		for (int deltaX = -r; deltaX <= r; deltaX++)
		for (int deltaY = -r; deltaY <= r; deltaY++)
		for (int deltaZ = -r; deltaZ <= r; deltaZ++)
		{
			if (deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ > r * r)
				continue;

			int bx = caveX + deltaX;
			int by = caveY + deltaY;
			int bz = caveZ + deltaZ;

			if (bx < 0 || bx >= Chunk::SIZE_X)
				continue;
			if (bz < 0 || bz >= Chunk::SIZE_Z)
				continue;
			if (by < 1)
				continue;
			if (by >= heightMap[bx][bz] - 2)
				continue;
			if (chunk->blocks[bx][bz][by] == WATER)
				continue;

			chunk->blocks[bx][bz][by] = AIR;
		}
	}
}

float TerrainGenerator::bilinearLerp(float grid[NoiseConstants::NOISE_GRID][NoiseConstants::NOISE_GRID],
	int intX, int intZ, float fractionX, float fractionZ)
{
	fractionX = fractionX * fractionX * (3.0f - 2.0f * fractionX);
	fractionZ = fractionZ * fractionZ * (3.0f - 2.0f * fractionZ);
	return grid[intX][intZ] * (1 - fractionX) * (1 - fractionZ) +
		grid[intX + 1][intZ] * fractionX * (1 - fractionZ) + 
		grid[intX][intZ + 1] * (1 - fractionX) * fractionZ + 
		grid[intX + 1][intZ + 1] * fractionX * fractionZ;
}
