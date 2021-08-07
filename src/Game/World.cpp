#include "World.h"

sf::Vector2f tileToScreenPosition(int x, int y)
{
    return {(WORLD_ORIGIN_OFFSET.x * TILE_WIDTH) + (x - y) * (TILE_WIDTH / 2.0f),
            (WORLD_ORIGIN_OFFSET.y * TILE_HEIGHT) + (x + y) * (TILE_HEIGHT / 2.0f)};
}