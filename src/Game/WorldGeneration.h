#pragma once

#include "Map.h"
#include <SFML/System/Vector2.hpp>

/*
std::vector<Tile> generateWorld(const sf::Vector2i& chunkPosition, int worldSize,
                                Map* map);
                                */

std::vector<Tile> generateWorld(const sf::Vector2i& chunkPosition, int seed);