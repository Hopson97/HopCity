#include "World.h"

#include "Map.h"
#include <glm/gtc/noise.hpp>
#include <iostream>
#include <numeric>
#include <random>

sf::Vector2f tileToScreenPosition(int worldSize, const sf::Vector2i& tilePosition)
{
    int x = tilePosition.x;
    int y = tilePosition.y;
    return {(worldSize * TILE_WIDTH) + (x - y) * (TILE_WIDTH / 2.0f),
            (worldSize * TILE_HEIGHT) + (x + y) * (TILE_HEIGHT / 2.0f)};
}

struct TerrainGenOptions {
    int octaves = 8;
    float amplitude = 230;
    float smoothness = 500;
    float roughness = 0.58;
    float offset = 0;

    int seed = 0;
};

float getNoiseAt(const sf::Vector2i& tilePosition, const sf::Vector2i& chunkPosition,
                 const TerrainGenOptions& options, int worldSize)
{
    // Get voxel X/Z positions
    // float voxelX = voxelPosition.x + chunkPosition.x * CHUNK_SIZE;
    // float voxelZ = voxelPosition.y + chunkPosition.y * CHUNK_SIZE;

    float tileX = (float)(tilePosition.x + chunkPosition.x * worldSize);
    float tileY = (float)(tilePosition.y + chunkPosition.y * worldSize);

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

std::vector<Tile> generateWorld(const sf::Vector2i& chunkPosition, int worldSize)
{

    std::vector<Tile> tiles(worldSize * worldSize);

    TerrainGenOptions ops;
    ops.amplitude = 20;
    ops.octaves = 4;
    ops.smoothness = 100;
    ops.roughness = 0.55f;

    std::random_device rd;
    std::mt19937 rng{rd()};
    std::uniform_real_distribution<float> pointDist(2, 6);
    std::uniform_int_distribution<int> dirDist(1, 2);
    std::uniform_int_distribution<int> seedDist(0, 4096);

    ops.seed = seedDist(rng);
    float riverPoint = pointDist(rng);
    float nsOceanSize = pointDist(rng);
    float ewOceanSize = pointDist(rng);
    bool isEast = dirDist(rng) == 1;
    bool isSouth = dirDist(rng) == 1;
    int riverDirection = dirDist(rng);
    float ws = static_cast<float>(worldSize);

    for (int y = 0; y < worldSize; y++) {
        for (int x = 0; x < worldSize; x++) {
            int rd = riverDirection == 1 ? x : y;

            std::vector<float> features;

            // Oceans
            features.push_back(std::abs(x - (isEast ? 0 : ws)) / ws * ewOceanSize);
            features.push_back(std::abs(y - (isSouth ? 0 : ws)) / ws * nsOceanSize);

            // River
            features.push_back(std::abs(rd - ws / riverPoint) / ws * 2);

            // Noise
            features.push_back(getNoiseAt({x, y}, chunkPosition, ops, worldSize));

            float f = std::accumulate(features.begin(), features.end(), 1.0f,
                                      std::multiplies<float>());
            tiles[x + y * worldSize].type = f > 0.4 ? TileType::Grass : TileType::Water;
        }
    }
    return tiles;
}