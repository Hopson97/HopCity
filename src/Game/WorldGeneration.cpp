#include "WorldGeneration.h"
#include "StructureMap.h"
#include <glm/gtc/noise.hpp>
#include <iostream>
#include <numeric>
#include <random>

#include "Common.h"
struct TerrainGenOptions {
    int octaves = 8;
    float amplitude = 230;
    float smoothness = 500;
    float roughness = 0.58;
    float offset = 0;

    int seed = 0;
};

float getNoiseAt(const sf::Vector2i& tilePosition, const sf::Vector2i& chunkPosition,
                 const TerrainGenOptions& options)
{
    // Get voxel X/Z positions
    // float voxelX = voxelPosition.x + chunkPosition.x * CHUNK_SIZE;
    // float voxelZ = voxelPosition.y + chunkPosition.y * CHUNK_SIZE;

    float tileX = (float)(tilePosition.x + chunkPosition.x * CHUNK_SIZE);
    float tileY = (float)(tilePosition.y + chunkPosition.y * CHUNK_SIZE);

    // Begin iterating through the octaves
    float value = 0;
    float accumulatedAmps = 0;
    for (int i = 0; i < options.octaves; i++) {
        float frequency = (float)glm::pow(2.0f, i);
        float amplitude = (float)glm::pow(options.roughness, i);

        float x = options.seed + tileX * frequency / options.smoothness;
        float y = options.seed + tileY * frequency / options.smoothness;

        float noise = (glm::simplex(glm::vec3{x, y, options.seed}) + 1.0f) / 2.0f;
        value += noise * amplitude;
        accumulatedAmps += amplitude;
    }
    return value / accumulatedAmps;
}

void generateTerrainForChunk(TileChunk* chunk, StructureMap* structMap, int seed)
{

    TerrainGenOptions ops;
    ops.amplitude = 20;
    ops.octaves = 4;
    ops.smoothness = 100;
    ops.roughness = 0.55f;

    std::random_device rd;
    std::mt19937 rng{rd()};
    std::uniform_real_distribution<float> pointDist(2, 6);
    std::uniform_int_distribution<int> dirDist(1, 2);

    ops.seed = seed;

    for (int y = 0; y < CHUNK_SIZE; y++) {
        for (int x = 0; x < CHUNK_SIZE; x++) {
            // int rd = riverDirection == 1 ? x : y;

            std::vector<float> features;
            // Noise
            float n = getNoiseAt({x, y}, chunk->chunkPosition, ops);
            features.push_back(n);

            float f = std::accumulate(features.begin(), features.end(), 1.0f, std::multiplies<float>());

            chunk->setTile({x, y}, f > 0.4 ? TileType::Land : TileType::Water);

            if (f > 0.4 && pointDist(rng) > 4 && n > 0.6) {
                structMap->placeStructure(StructureType::FirTree,
                                          toGlobalTilePosition(chunk->chunkPosition, {x, y}),
                                          *chunk->p_chunkManager);
            }
        }
    }
}
