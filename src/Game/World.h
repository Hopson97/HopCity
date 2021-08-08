#pragma once

#include <SFML/System/Vector2.hpp>

constexpr int WORLD_SIZE = 250;

constexpr float TILE_HEIGHT = 16.0f;
constexpr float TILE_WIDTH = 32.0f;

const sf::Vector2f WORLD_ORIGIN_OFFSET{WORLD_SIZE, WORLD_SIZE};

sf::Vector2f tileToScreenPosition(const sf::Vector2i& tilePosition);