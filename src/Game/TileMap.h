#pragma once

#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include "../Animation.h"

enum class TileType : uint8_t {
    Grass,
    Road,
    Water,
};

struct Tile {
    TileType type = TileType::Grass;
    uint8_t varient = 0;
};

struct TileMap {
  public:
    TileMap();

    Tile* getTile(const sf::Vector2i& position);
    void updateTile(const sf::Vector2i& position);

    void renderTiles(sf::RenderWindow* window);

  private:
    sf::Texture m_tilemap;
    std::vector<Tile> m_tiles;
    std::vector<sf::Vertex> m_grid;
    std::vector<sf::Vertex> m_tileVerts;
    std::vector<sf::Vertex> m_waterAnimationVerts;
    Animation m_wateranim;
};