#pragma once

#include "Map.h"
#include <SFML/System/Vector2.hpp>
#include "FastNoiseLite.h"

constexpr float TILE_HEIGHT = 16.0f;
constexpr float TILE_WIDTH = 32.0f;

sf::Vector2f tileToScreenPosition(int worldSize, const sf::Vector2i& tilePosition);

static struct TerrainGenOptions {
    int octaves = 8;
    float amplitude = 1.0;
    float smoothness = 500;
    float frequency = 0.04;
    float offset = 0;

    int seed;
    int useGreedyMeshing = true;
} terrainGenOptions;


std::vector<Tile> generateWorld(const sf::Vector2i& chunkPosition, int worldSize);