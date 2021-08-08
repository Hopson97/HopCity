#include "ScreenGame.h"
#include "ScreenMainMenu.h"
#include <cmath>
#include <imgui_sfml/imgui.h>
#include <iostream>

ScreenGame::ScreenGame(ScreenManager* stack)
    : Screen(stack)

{
    m_selectionTexture.loadFromFile("Data/Textures/Selection.png");
    m_selectionRedTexture.loadFromFile("Data/Textures/SelectionRed.png");
    m_tileCorners.loadFromFile("Data/Textures/Corners.png");

    m_selectionRect.setSize({TILE_WIDTH, TILE_HEIGHT});
}

void ScreenGame::onInput(const Keyboard& keyboard, const sf::RenderWindow& window)
{
    auto& ts = profiler.requestTimeslot("Input");
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

    if (m_quadDrag) {
        m_editEndPosition = m_selectedTile;
    }
    ts.stop();
}

void ScreenGame::onGUI()
{
    auto& ts = profiler.requestTimeslot("GUI");
    if (ImGui::Begin("Info")) {
        ImGui::Text("Tile: %d %d", m_selectedTile.x, m_selectedTile.y);
        ImGuiIO& io = ImGui::GetIO();
        ImGui::Text("Performance %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate,
                    io.Framerate);
    }
    ImGui::End();
    profiler.onGUI();
    ts.stop();
}

void ScreenGame::onEvent(const sf::Event& e)
{
    auto& ts = profiler.requestTimeslot("Event");
    m_camera.onEvent(e);
    if (e.type == sf::Event::MouseButtonPressed) {
        m_quadDrag = true;
        m_editStartPosition = m_selectedTile;
    }
    else if (e.type == sf::Event::MouseButtonReleased) {
        m_quadDrag = false;

        forEachSelectedTile([&](const sf::Vector2i& tilepos) {
            Tile* tile = m_map.getTile(tilepos);
            tile->type =
                e.mouseButton.button == sf::Mouse::Left
                    ? (sf::Keyboard::isKeyPressed(sf::Keyboard::Q) ? TileType::Road
                                                                   : TileType::Water)
                    : TileType::Grass;

            for (int i = -1; i <= 1; i++) {
                for (int j = -1; j <= 1; j++) {
                    m_map.updateTile(tilepos + sf::Vector2i{i, j});
                }
            }
        });
    }
    ts.stop();
}

void ScreenGame::onUpdate(const sf::Time& dt) {
    auto& ts = profiler.requestTimeslot("Update");
    ts.stop();
}

void ScreenGame::onRender(sf::RenderWindow* window)
{
    auto& ts = profiler.requestTimeslot("Render");
    m_camera.setViewToCamera(*window);

    // Render the tile map
    m_map.renderTiles(window, m_camera.zoomLevel < 2);

    // Render the selected tile
    m_selectionRect.setTexture(&m_selectionTexture);
    m_selectionRect.setPosition(tileToScreenPosition(m_selectedTile));
    window->draw(m_selectionRect);

    if (m_quadDrag) {
        m_selectionRect.setTexture(&m_selectionRedTexture);
        forEachSelectedTile([&](const sf::Vector2i& tile) {
            m_selectionRect.setPosition(tileToScreenPosition(tile));
            window->draw(m_selectionRect);
        });
    }
    ts.stop();
}

void ScreenGame::forEachSelectedTile(std::function<void(const sf::Vector2i& tile)> f)
{
    int startX = std::min(m_editStartPosition.x, m_editEndPosition.x);
    int startY = std::min(m_editStartPosition.y, m_editEndPosition.y);
    int endX = std::max(m_editStartPosition.x, m_editEndPosition.x + 1);
    int endY = std::max(m_editStartPosition.y, m_editEndPosition.y + 1);
    for (int y = startY; y < endY; y++) {
        for (int x = startX; x < endX; x++) {
            f({x, y});
        }
    }
}