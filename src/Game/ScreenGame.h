#pragma once

#include "../Animation.h"
#include "../Screen.h"
#include "Camera.h"
#include "Construction.h"
#include "Renderer.h"
#include "Resources.h"
#include "StructureMap.h"
#include "TileMap.h"
#include "WorldGeneration.h"
#include <functional>

class ScreenGame final : public Screen {
  public:
    ScreenGame(ScreenManager* screens);

    void onGUI() override;
    void onInput(const Keyboard& keyboard, const sf::RenderWindow& window) override;
    void onEvent(const sf::Event& e) override;
    void onUpdate(const sf::Time& dt) override;
    void onFixedUpdate(const sf::Time& dt) override;
    void onRender(sf::RenderWindow* window) override;

  private:
    sf::RectangleShape m_selectionRect;
    sf::Texture m_selectionTexture;
    sf::Texture m_selectionQuadTexture;

    sf::Image m_tileCorners;

    sf::Vector2i m_selectedTile;
    bool m_mousedown = false;
    sf::Mouse::Button m_buttonPressed;

    TileMap m_tileManager;
    StructureMap m_structureMap;

    // Map m_map;
    Camera m_camera;

    // Editor
    sf::Vector2i m_editStartPosition;
    sf::Vector2i m_editPivotPoint;
    sf::Vector2i m_editEndPosition;
    bool m_isConstructing = false;

    Resources m_resources;

    int m_seed = 52323;

    int m_team = 1;

    Renderer m_levelRenderer;
};
