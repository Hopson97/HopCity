#pragma once

#include <SFML/System/Vector2.hpp>
#include <cstdint>

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

struct Vec2Compare {
    inline bool operator()(const sf::Vector2i& l, const sf::Vector2i& r) const
    {
        return l.x < r.x || l.y < r.y;
    }
};

inline sf::Vector2f tileToScreenPosition(int worldSize, const sf::Vector2i& tilePosition)
{
    int x = tilePosition.x;
    int y = tilePosition.y;
    return {(worldSize * TILE_WIDTH) + (x - y) * (TILE_WIDTH / 2.0f),
            (worldSize * TILE_HEIGHT) + (x + y) * (TILE_HEIGHT / 2.0f)};
}
