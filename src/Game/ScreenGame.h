#pragma once

#include "../Animation.h"
#include "../Screen.h"
#include "Camera.h"
#include "TileMap.h"
#include "World.h"

class ScreenGame final : public Screen {
  public:
    ScreenGame(ScreenManager* screens);

    void onGUI() override;
    void onInput(const Keyboard& keyboard, const sf::RenderWindow& window) override;
    void onEvent(const sf::Event& e) override;
    void onUpdate(const sf::Time& dt) override;
    void onRender(sf::RenderWindow* window) override;

  private:
    sf::RectangleShape m_selectionRect;
    sf::Texture m_selectionTexture;

    sf::Image m_tileCorners;

    sf::Vector2i m_selectedTile;
    bool m_mousedown = false;
    sf::Mouse::Button m_buttonPressed;
    Camera m_camera;

    TileMap m_map;
};
