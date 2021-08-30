#pragma once

#include "Registry/StructureRegistry.h"
#include "Registry/TileRegistry.h"
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/VertexBuffer.hpp>
#include <unordered_map>

class TileMap;

class StructureMap {
  public:
    StructureMap();

    void clear();

    void draw(sf::RenderWindow* target);

    bool isStructureAt(const sf::Vector2i& tilePosition);
    StructureType removeStructure(const sf::Vector2i& tilePosition, TileMap& manager);

    void setCurrentlySelectedTile(const sf::Vector2i& position);
    const Structure& getStructure(const sf::Vector2i& position);
    void placeStructure(StructureType type, const sf::Vector2i& position, TileMap& manager);

  private:
    std::unordered_map<sf::Vector2i, Structure, Vec2hash> m_structures;
    std::vector<sf::Vector2i> m_sortedStructList;

    sf::RectangleShape m_structureRect;
    sf::Texture m_structureMap;

    sf::Vector2i m_currentlySelectedTile;
};