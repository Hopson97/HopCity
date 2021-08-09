#pragma once

#include <SFML/System/Vector2.hpp>

constexpr float TILE_HEIGHT = 16.0f;
constexpr float TILE_WIDTH = 32.0f;

sf::Vector2f tileToScreenPosition(int worldSize, const sf::Vector2i& tilePosition);