#pragma once

#include "Map.h"
#include <SFML/System/Vector2.hpp>
#include "WorldConstants.h"


std::vector<Tile> generateWorld(const sf::Vector2i& chunkPosition, int worldSize,
                                Map* map);