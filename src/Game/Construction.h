#pragma once

#include "Registry/StructureRegistry.h"
#include <functional>

void onConstructionGUI();

/**
 * @brief Calls a function for a L shape selection (for example, building a wall)
 *
 * @param start The tile position to start at
 * @param mid  The tile position to "pivot" the L at
 * @param end The tile position to end end
 * @param f Callback for each section along the L shape
 */
void forEachLSection(const sf::Vector2i& start, const sf::Vector2i& mid, const sf::Vector2i& end,
                     std::function<void(const sf::Vector2i& tile)> f);

void forEachQuadSection(const sf::Vector2i& start, const sf::Vector2i& end,
                        std::function<void(const sf::Vector2i& tile)> f);