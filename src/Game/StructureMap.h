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
class Renderer;

class StructureMap {
  public:
    void clear();

    void draw(Renderer& renderer);

    bool isStructureAt(const sf::Vector2i& tilePosition);
    int structureTeamAt(const sf::Vector2i& tilePosition);

    StructureType removeStructure(const sf::Vector2i& tilePosition, TileMap& manager);

    const Structure& getStructure(const sf::Vector2i& position);
    void placeStructure(StructureType type, const sf::Vector2i& position, TileMap& manager, int team);

  private:
    std::unordered_map<sf::Vector2i, Structure, Vec2hash> m_structures;
};