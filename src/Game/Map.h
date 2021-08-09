#pragma once

#include "../Animation.h"
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/VertexBuffer.hpp>
#include <imgui_sfml/imgui.h>


enum class TileType : uint8_t {
    Grass,
    Road,
    Water,
};

struct Tile {
    TileType type = TileType::Water;
    uint8_t varient = 0;
};

struct Map {
  public:
    Map(int worldSize);

    void setTile(const sf::Vector2i& position, TileType type);
    void draw(sf::RenderWindow* target);
    void regenerate();
    void onGUI();
    bool showDetail;

  private:
    Tile* getTile(const sf::Vector2i& position);
    void updateTile(const sf::Vector2i& position);

    sf::Texture m_tileTextures;
    std::vector<Tile> m_tiles;
    std::vector<sf::Vertex> m_grid;
    std::vector<sf::Vertex> m_foregroundTileVerticies;
    std::vector<sf::Vertex> m_backgroundTileVerticies;

    Animation m_waterAnimation;

    int m_worldSize;
};