#pragma once

#include "../Common.h"
#include <array>
#include <cstdint>

enum class TileType : uint8_t
{
    Land,
    Road,
    Water,

    NUM_TILES,
};

struct Tile
{
    TileType type = TileType::Water;
    uint8_t variant = 0;
};

struct TileDef
{
    int textureIndex = 0;

    VairantType variantType = VairantType::None;
    int variations = 0; // For Random variation
};

class TileRegistry
{
  public:
    static TileRegistry& instance();

    const TileDef& getTileDef(TileType type);

  private:
    TileRegistry();

    void registerTiles();
    TileDef& registerTile(TileType type, int textureIndex, VairantType variance);

    std::array<TileDef, (std::size_t)TileType::NUM_TILES> m_tiles;
};
