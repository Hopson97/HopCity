#pragma once

#include "../Animation.h"
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/VertexBuffer.hpp>

enum class TileType : uint8_t {
    Land,
    Road,
    Water,

    Building,
};

enum class StructureType {
    FirTree,
};

struct Tile {
    TileType type = TileType::Water;
    uint8_t varient = 0;
};

struct Structure {
    StructureType type;
    sf::Vector2i tilePosition;
};

struct Map {
  public:
    Map(int worldSize);

    void regenerateTerrain();

    void setTile(const sf::Vector2i& position, TileType type);

    void draw(sf::RenderWindow* target);

    bool showDetail;

    void placeStructure(StructureType type, const sf::Vector2i& position);

  private:
    Tile* getTile(const sf::Vector2i& position);
    void updateTile(const sf::Vector2i& position);

    sf::Texture m_tileTextures;
    std::vector<Tile> m_tiles;
    std::vector<sf::Vertex> m_grid;
    std::vector<sf::Vertex> m_tileVerts;

    std::vector<Structure> m_structures;

    sf::RectangleShape m_structureRect;
    sf::Texture m_structureMap;

    int m_worldSize;
};