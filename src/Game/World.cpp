#include "World.h"

#include "Map.h"
#include <iostream>
#include <glm/gtc/noise.hpp>

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

    int seed;
    int useGreedyMeshing = true;
};

float getNoiseAt(const sf::Vector2i& tilePosition, const sf::Vector2i& chunkPosition,
                 const TerrainGenOptions& options, int worldSize)
{
    // Get voxel X/Z positions
    // float voxelX = voxelPosition.x + chunkPosition.x * CHUNK_SIZE;
    // float voxelZ = voxelPosition.y + chunkPosition.y * CHUNK_SIZE;

    float tileX = tilePosition.x + chunkPosition.x * worldSize;
    float tileY = tilePosition.y + chunkPosition.y * worldSize;

    // Begin iterating through the octaves
    float value = 0;
    float accumulatedAmps = 0;
    for (int i = 0; i < options.octaves; i++) {
        float frequency = glm::pow(2.0f, i);
        float amplitude = glm::pow(options.roughness, i);

        float x = tileX * frequency / options.smoothness;
        float y = tileY * frequency / options.smoothness;

        float noise =
            glm::simplex(glm::vec3{options.seed + x, options.seed + y, options.seed});
        noise = (noise + 1.0f) / 2.0f;
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
    ops.roughness = 0.55;
    ops.offset = 0;
    srand(std::time(nullptr));
    ops.seed = rand() % 1024;

    for (int y = 0; y < worldSize; y++) {
        for (int x = 0; x < worldSize; x++) {
            float noise = getNoiseAt({x, y}, chunkPosition, ops, worldSize);

            float dist1 = (float)std::abs(x - worldSize) / (float)worldSize *4;
            float dist2 = (float)std::abs(y - worldSize) / (float)worldSize * 4;
            float river = (float)std::abs(y - worldSize / 4) / (float)worldSize * 2;


            float final = dist1 * dist2 * river * noise;
    
            tiles[x + y * worldSize].type =
                final > 0.4 ? TileType::Grass : TileType::Water;
        }
    }
    return tiles;
}