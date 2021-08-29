#pragma once

#include "TileMap.h"
#include <SFML/System/Vector2.hpp>

std::vector<Tile> generateWorld(const sf::Vector2i& chunkPosition, TileChunkManager* map,
                                int seed);
