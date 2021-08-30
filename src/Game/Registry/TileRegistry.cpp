#include "TileRegistry.h"

TileRegistry& TileRegistry::instance()
{
    static TileRegistry reg;
    return reg;
}

TileRegistry::TileRegistry()
{
    registerTile(TileType::Land, 0, VairantType::None);
    registerTile(TileType::Road, 1, VairantType::Neighbour);
    registerTile(TileType::Water, 2, VairantType::Neighbour);
}

TileDef& TileRegistry::registerTile(TileType type, int textureIndex, VairantType variance)
{
    TileDef def;
    def.variantType = variance;
    def.textureIndex = textureIndex;

    m_tiles[(std::size_t)type] = def;
    return m_tiles[(std::size_t)type];
}

const TileDef& TileRegistry::getTileDef(TileType type)
{
    return m_tiles[(std::size_t)type];
}
