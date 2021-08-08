#pragma once

#include <SFML/System/Vector2.hpp>

constexpr int WORLD_SIZE = 128;

constexpr float TILE_HEIGHT = 16.0f;
constexpr float TILE_WIDTH = 32.0f;

const sf::Vector2f WORLD_ORIGIN_OFFSET{WORLD_SIZE, WORLD_SIZE};

sf::Vector2f tileToScreenPosition(int x, int y);