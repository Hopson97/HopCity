#pragma once

#include <SFML/System/Vector2.hpp>

#include "TileMap.h"

class StructureMap;

void generateTerrainForChunk(TileChunk* chunk, StructureMap* structMap, int seed);
