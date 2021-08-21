#pragma once

#include "../Animation.h"
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/VertexBuffer.hpp>
#include <set>

struct Vec2hash {
    inline size_t operator()(const sf::Vector2i& v) const
    {
        return (static_cast<size_t>(v.x) << 32) | static_cast<size_t>(v.y);
    }
};

struct Vec2Compare {
    inline bool operator()(const sf::Vector2i& l, const sf::Vector2i& r) const
    {
        return l.x < r.x || l.y < r.y;
    }
};

enum class TileType : uint8_t {
    Land,
    Road,
    Water,

    Building,
};

enum class StructureType { FirTree, Wall };

struct Tile {
    TileType type = TileType::Water;
    uint8_t varient = 0;
};

struct Structure {
    StructureType type;
    uint8_t varient = 0;
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

    std::unordered_map<sf::Vector2i, Structure, Vec2hash> m_structures;
    std::set<sf::Vector2i, Vec2Compare> sorted;

    sf::RectangleShape m_structureRect;
    sf::Texture m_structureMap;

    int m_worldSize;
};