#pragma once

#include "../Animation.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Drawable.hpp>

enum class TileType : uint8_t {
    Grass,
    Road,
    Water,
};

struct Tile {
    TileType type = TileType::Grass;
    uint8_t varient = 0;
};

struct TileMapRenderer : public sf::Drawable, private sf::Transformable {
  public:
    TileMapRenderer();

    Tile* getTile(const sf::Vector2i& position);
    void updateTile(const sf::Vector2i& position);

    void updateAnimation();
    void setDetail(bool showDetail);
    void draw(sf::RenderTarget& target, sf::RenderStates states) const;

  private:
    sf::Texture m_tileTextures;
    std::vector<Tile> m_tiles;
    std::vector<sf::Vertex> m_grid;
    std::vector<sf::Vertex> m_foregroundTileVerticies;
    std::vector<sf::Vertex> m_backgroundTileVerticies;
    Animation m_waterAnimation;

    bool m_showDetail;
};