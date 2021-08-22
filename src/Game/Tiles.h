#pragma once

#include "Common.h"
#include <cstdint>

enum class TileType : uint8_t {
    Land,
    Road,
    Water,

    NUM_TILES,
};

struct Tile {
    TileType type = TileType::Water;
    uint8_t variant = 0;
};

struct TileDef {
    int textureIndex = 0;

    VairantType variantType = VairantType::None;
    int variations = 0; // For Random variation
};

void registerTiles();
TileDef& registerTile(TileType type, int textureIndex, VairantType variance);
const TileDef& getTileDef(TileType type);