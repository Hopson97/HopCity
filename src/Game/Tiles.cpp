#include "Tiles.h"

#include <array>

std::array<TileDef, (size_t)TileType::NUM_TILES> tiles;

void registerTiles()
{
    registerTile(TileType::Land, 0, VairantType::None);
    registerTile(TileType::Road, 1, VairantType::Neighbour);
    registerTile(TileType::Water, 2, VairantType::Neighbour);
}

TileDef& registerTile(TileType type, int textureIndex, VairantType variance)
{
    TileDef def;
    def.variantType = variance;
    def.textureIndex = textureIndex;

    tiles[(size_t)type] = def;
    return tiles[(size_t)type];
}

const TileDef& getTileDef(TileType type) { return tiles[(size_t)type]; }
