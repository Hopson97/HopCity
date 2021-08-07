#pragma once

#include "../Animation.h"
#include "../Screen.h"
#include "World.h"
#include "Camera.h"

enum class TileType : uint8_t {
    Grass,
    Road,
    Water,
};

struct Tile {
    TileType type = TileType::Grass;
    uint8_t varient = 0;
};

class ScreenGame final : public Screen {
  public:
    ScreenGame(ScreenManager* screens);

    void onGUI() override;
    void onInput(const Keyboard& keyboard, const sf::RenderWindow& window) override;
    void onEvent(const sf::Event& e) override;
    void onUpdate(const sf::Time& dt) override;
    void onRender(sf::RenderWindow* window) override;

  private:
    Tile* getTile(const sf::Vector2i& position);
    void updateTileTextureCoords(const sf::Vector2i& position);
    void autoTile(const sf::Vector2i& position);

    sf::RectangleShape m_tileRect;
    sf::Texture m_tilemap;
    sf::Texture m_selectionTexture;
    

    sf::Image m_tileCorners;

    std::vector<Tile> m_tiles;
    std::vector<sf::Vertex> m_grid;
    std::vector<sf::Vertex> m_tileVerts;
    std::vector<sf::Vertex> m_waterAnimationVerts;

    sf::Vector2i m_selectedTile;
    bool m_mousedown = false;
    sf::Mouse::Button m_buttonPressed;

    bool drawGrid = true;

    Camera m_camera;

    
    Animation m_wateranim;
};
