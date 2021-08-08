#include "World.h"

sf::Vector2f tileToScreenPosition(const sf::Vector2i& tilePosition)
{
    int x = tilePosition.x;
    int y = tilePosition.y;
    return {(WORLD_ORIGIN_OFFSET.x * TILE_WIDTH) + (x - y) * (TILE_WIDTH / 2.0f),
            (WORLD_ORIGIN_OFFSET.y * TILE_HEIGHT) + (x + y) * (TILE_HEIGHT / 2.0f)};
}