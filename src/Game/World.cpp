#include "World.h"

#include "Map.h"
#include <iostream>

sf::Vector2f tileToScreenPosition(int worldSize, const sf::Vector2i& tilePosition)
{
    int x = tilePosition.x;
    int y = tilePosition.y;
    return {(worldSize * TILE_WIDTH) + (x - y) * (TILE_WIDTH / 2.0f),
            (worldSize * TILE_HEIGHT) + (x + y) * (TILE_HEIGHT / 2.0f)};
}



float getNoiseAt(const sf::Vector2i& tilePosition, const sf::Vector2i& chunkPosition,
                 const TerrainGenOptions& options, int worldSize)
{
    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    noise.SetFractalOctaves(options.octaves);
    noise.SetSeed(options.seed);
    noise.SetFrequency(options.frequency); //i guess??

    return noise.GetNoise((float)tilePosition.x,(float)tilePosition.y
                      );
}

std::vector<Tile> generateWorld(const sf::Vector2i& chunkPosition, int worldSize)
{
    
    std::vector<Tile> tiles(worldSize * worldSize);

    TerrainGenOptions ops = terrainGenOptions;
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