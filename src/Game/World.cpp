#include "World.h"

sf::Vector2f tileToScreenPosition(int worldSize, const sf::Vector2i& tilePosition)
{
    int x = tilePosition.x;
    int y = tilePosition.y;
    return {(worldSize * TILE_WIDTH) + (x - y) * (TILE_WIDTH / 2.0f),
            (worldSize * TILE_HEIGHT) + (x + y) * (TILE_HEIGHT / 2.0f)};
}