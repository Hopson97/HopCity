#include "ScreenGame.h"
#include "ScreenMainMenu.h"
#include <cmath>
#include <imgui_sfml/imgui.h>
#include <iostream>

ScreenGame::ScreenGame(ScreenManager* stack)
    : Screen(stack)

{
    m_selectionTexture.loadFromFile("Data/Textures/Selection.png");
    m_tileCorners.loadFromFile("Data/Textures/Corners.png");

    m_selectionRect.setTexture(&m_selectionTexture);
    m_selectionRect.setSize({TILE_WIDTH, TILE_HEIGHT});
}

void ScreenGame::onInput(const Keyboard& keyboard, const sf::RenderWindow& window)
{
    m_camera.onInput(keyboard, window);
    auto mousePosition = sf::Mouse::getPosition(window);
    sf::Vector2f worldPos = window.mapPixelToCoords(mousePosition);

    sf::Vector2i cell = {(int)worldPos.x / (int)TILE_WIDTH,
                         (int)worldPos.y / (int)TILE_HEIGHT};
    sf::Vector2i offset = {(int)worldPos.x % (int)TILE_WIDTH,
                           (int)worldPos.y % (int)TILE_HEIGHT};

    m_selectedTile = {
        (cell.y - (int)WORLD_ORIGIN_OFFSET.y) + (cell.x - (int)WORLD_ORIGIN_OFFSET.x),
        (cell.y - (int)WORLD_ORIGIN_OFFSET.y) - (cell.x - (int)WORLD_ORIGIN_OFFSET.x),
    };

    sf::Color colour = m_tileCorners.getPixel(offset.x, offset.y);
    if (colour == sf::Color::Red)
        m_selectedTile.x--;
    else if (colour == sf::Color::Green)
        m_selectedTile.y++;
    else if (colour == sf::Color::Blue)
        m_selectedTile.y--;
    else if (colour == sf::Color::White)
        m_selectedTile.x++;
}

void ScreenGame::onGUI()
{
    if (ImGui::Begin("Info")) {
        ImGui::Text("Tile: %d %d", m_selectedTile.x, m_selectedTile.y);
        ImGui::Checkbox("Draw Grid", &drawGrid);
    }
    ImGui::End();
    ImGui::ShowMetricsWindow(nullptr);
}

void ScreenGame::onEvent(const sf::Event& e)
{
    m_camera.onEvent(e);
    if (e.type == sf::Event::MouseButtonPressed) {
        m_mousedown = true;
        m_buttonPressed = e.mouseButton.button;
    }
    else if (e.type == sf::Event::MouseButtonReleased) {
        m_mousedown = false;
    }

    if (m_mousedown) {
        Tile* tile = m_map.getTile(m_selectedTile);
        if (m_buttonPressed == sf::Mouse::Left) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
                tile->type = TileType::Water;
            }
            else {
                tile->type = TileType::Road;
            }
        }
        else if (m_buttonPressed == sf::Mouse::Right) {
            tile->type = TileType::Grass;
        }

        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                m_map.updateTile(m_selectedTile + sf::Vector2i{i, j});
            }
        }
    }
}

void ScreenGame::onUpdate(const sf::Time& dt) {}

void ScreenGame::onRender(sf::RenderWindow* window)
{
    m_camera.setViewToCamera(*window);

    // Render the tile map
    m_map.renderTiles(window);

    // Render the selected tile
    m_selectionRect.setPosition(tileToScreenPosition(m_selectedTile.x, m_selectedTile.y));
    window->draw(m_selectionRect);
}
