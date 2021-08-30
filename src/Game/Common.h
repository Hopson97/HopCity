#pragma once

#include <SFML/System/Vector2.hpp>
#include <cstdint>

constexpr int CHUNK_SIZE = 128;

constexpr float TILE_HEIGHT = 16.0f;
constexpr float TILE_WIDTH = 32.0f;

enum class VairantType {
    None = 0,
    Neighbour = 1,
    Random = 2,
};

struct Vec2hash {
    inline std::size_t operator()(const sf::Vector2i& v) const
    {
        return (static_cast<std::size_t>(v.x) << 32) | static_cast<std::size_t>(v.y);
    }
};

inline sf::Vector2f tileToScreenPosition(const sf::Vector2i& tilePosition)
{
    int x = tilePosition.x;
    int y = tilePosition.y;
    return {(CHUNK_SIZE * TILE_WIDTH) + (x - y) * (TILE_WIDTH / 2.0f),
            (CHUNK_SIZE * TILE_HEIGHT) + (x + y) * (TILE_HEIGHT / 2.0f)};
}

inline sf::Vector2i toChunkPosition(const sf::Vector2i& tilePosition)
{
    return {tilePosition.x / CHUNK_SIZE, tilePosition.y / CHUNK_SIZE};
}

inline sf::Vector2i toLocalTilePosition(const sf::Vector2i& worldTilePosition)
{
    return {worldTilePosition.x % CHUNK_SIZE, worldTilePosition.y % CHUNK_SIZE};
}

inline sf::Vector2i toGlobalTilePosition(const sf::Vector2i& chunkPosition,
                                         const sf::Vector2i& localTilePosition)
{
    return {chunkPosition.x * CHUNK_SIZE + localTilePosition.x,
            chunkPosition.y * CHUNK_SIZE + localTilePosition.y};
}