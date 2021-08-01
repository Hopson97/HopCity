#pragma once

#include "Animation.h"
#include "Screen.h"

#include "Game/World.h"

enum class TileType { Grass, Road };

struct Tile {
    TileType type = TileType::Grass;
    int varient = 0;
    ;
};

class ScreenGame final : public Screen {
    enum class Menu {
        MainMenu,
        SettingsMenu,
    };

  public:
    ScreenGame(ScreenManager* screens);

    void onGUI() override;
    void onInput(const Keyboard& keyboard, const sf::RenderWindow& window) override;
    void onEvent(const sf::Event& e) override;
    void onUpdate(const sf::Time& dt) override;
    void onRender(sf::RenderWindow* window) override;

  private:
    void updateTileVarient(const sf::Vector2i& position);
    Tile* getTile(const sf::Vector2i& position);
    sf::Vector2f tileToScreenPosition(int x, int y);

    sf::View m_view;
    Menu m_activeMenu = Menu::MainMenu;

    sf::RectangleShape m_sprite;
    sf::Vector2f m_originOffset{WORLD_SIZE * 4, WORLD_SIZE * 4};

    sf::Texture m_tileTexture;
    sf::Image m_tileCorners;

    std::vector<Tile> m_tiles;
    std::vector<sf::Vertex> m_grid;

    sf::Texture m_roadTexture;

    sf::Vector2i m_selectedTile;
};
