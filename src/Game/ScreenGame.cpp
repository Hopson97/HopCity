#include "ScreenGame.h"
#include "ScreenMainMenu.h"
#include "Tiles.h"
#include <cmath>
#include <imgui_sfml/imgui.h>
#include <iostream>
#include <unordered_set>

#include "Common.h"

namespace {

    bool xDistGreater(const sf::Vector2i& startPoint, const sf::Vector2i& endPoint)
    {
        return std::abs(startPoint.x - endPoint.x) > std::abs(startPoint.y - endPoint.y);
    }

    /**
     * @brief Calls a function for a L shape selection (for example, building a road)
     *
     * @param start The tile position to start at
     * @param mid  The tile position to "pivot" the L at
     * @param end The tile position to end end
     * @param f Callback for each section along the L shape
     */
    void forEachLSection(const sf::Vector2i& start, const sf::Vector2i& mid,
                         const sf::Vector2i& end,
                         std::function<void(const sf::Vector2i& tile)> f)
    {

        // Hack to prevent the functions running for a tile position more than once
        std::unordered_set<sf::Vector2i, Vec2hash> unique;
        auto tryTile = [&](const sf::Vector2i pos) {
            if (!unique.count(pos)) {
                unique.emplace(pos);
                f(pos);
            }
        };

        if (xDistGreater(start, mid)) {
            int startX = std::min(start.x, mid.x);
            int startY = std::min(mid.y, end.y);

            int endX = std::max(start.x + 1, mid.x + 1);
            int endY = std::max(mid.y + 1, end.y + 1);

            for (int x = startX; x < endX; x++) {
                tryTile({x, start.y});
            }
            for (int y = startY; y < endY; y++) {
                tryTile({mid.x, y});
            }
        }
        else {
            int startX = std::min(mid.x, end.x);
            int startY = std::min(start.y, mid.y);
            int endX = std::max(mid.x + 1, end.x + 1);
            int endY = std::max(start.y + 1, mid.y + 1);

            for (int x = startX; x < endX; x++) {
                tryTile({x, mid.y});
            }
            for (int y = startY; y < endY; y++) {
                tryTile({start.x, y});
            }
        }
    }

    void forEachQuadSection(const sf::Vector2i& start, const sf::Vector2i& end,
                            std::function<void(const sf::Vector2i& tile)> f)
    {
        int startX = std::min(start.x, end.x);
        int startY = std::min(start.y, end.y);
        int endX = std::max(start.x + 1, end.x + 1);
        int endY = std::max(start.y + 1, end.y + 1);

        for (int y = startY; y < endY; y++) {
            for (int x = startX; x < endX; x++) {
                f({x, y});
            }
        }
    }

} // namespace

ScreenGame::ScreenGame(ScreenManager* stack)
    : Screen(stack)
{
    m_selectionTexture.loadFromFile("data/Textures/Selection.png");
    m_selectionQuadTexture.loadFromFile("data/Textures/SelectionQuad.png");
    m_tileCorners.loadFromFile("data/Textures/Corners.png");

    m_selectionRect.setSize({TILE_WIDTH, TILE_HEIGHT});

    registerStructures();
    registerTiles();

    m_tileManager.initWorld();
}

void ScreenGame::onInput(const Keyboard& keyboard, const sf::RenderWindow& window)
{
    m_camera.onInput(keyboard, window);

    if (!ImGui::GetIO().WantCaptureMouse) {
        auto mousePosition = sf::Mouse::getPosition(window);
        sf::Vector2f worldPos = window.mapPixelToCoords(mousePosition);

        sf::Vector2i cell = {(int)worldPos.x / (int)TILE_WIDTH,
                             (int)worldPos.y / (int)TILE_HEIGHT};
        sf::Vector2i offset = {(int)worldPos.x % (int)TILE_WIDTH,
                               (int)worldPos.y % (int)TILE_HEIGHT};

        m_selectedTile = {
            (cell.y - CHUNK_SIZE) + (cell.x - CHUNK_SIZE),
            (cell.y - CHUNK_SIZE) - (cell.x - CHUNK_SIZE),
        };

        // clang-format off
        if (offset.x >= 0 && 
            offset.y >= 0 && 
            offset.x < (int)m_tileCorners.getSize().x && 
            offset.y < (int)m_tileCorners.getSize().y)
        {
            sf::Color colour = m_tileCorners.getPixel(offset.x, offset.y);
                 if (colour == sf::Color::Red   )   m_selectedTile.x--;
            else if (colour == sf::Color::Green )   m_selectedTile.y++;
            else if (colour == sf::Color::Blue  )   m_selectedTile.y--;
            else if (colour == sf::Color::White )   m_selectedTile.x++;
        }
        // clang-format on
    }

    if (m_quadDrag) {
        if (xDistGreater(m_editStartPosition, m_editEndPosition)) {
            m_editPivotPoint.x = m_selectedTile.x;
            m_editPivotPoint.y = m_editStartPosition.y;
        }
        else {
            m_editPivotPoint.x = m_editStartPosition.x;
            m_editPivotPoint.y = m_selectedTile.y;
        }
        m_editEndPosition = m_selectedTile;
    }
}

void ScreenGame::onGUI()
{
    if (ImGui::Begin("Info")) {
        ImGui::Text("Tile: %d %d", m_selectedTile.x, m_selectedTile.y);
        ImGuiIO& io = ImGui::GetIO();
        ImGui::Text("Performance %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate,
                    io.Framerate);
        if (ImGui::Button("Regen world")) {
            m_tileManager.regenerateTerrain();
        }
    }
    ImGui::End();
}

void ScreenGame::onEvent(const sf::Event& e)
{
    m_camera.onEvent(e);
    if (!ImGui::GetIO().WantCaptureMouse) {
        if (e.type == sf::Event::MouseButtonPressed) {
            m_quadDrag = true;
            m_editStartPosition = m_selectedTile;
            m_editEndPosition = m_selectedTile;
        }
        else if (e.type == sf::Event::MouseButtonReleased) {
            if (m_tileManager.canPlaceStructure(m_selectedTile, StructureType::Base)) {
                m_tileManager.placeStructure(StructureType::Base, m_selectedTile);
            }

            m_quadDrag = false;

            /*
            forEachLSection(m_editStartPosition, m_editPivotPoint, m_editEndPosition,
                            [&](const sf::Vector2i& tilePosition) {
                                if (m_tileManager.canPlaceStructure(
                                        m_selectedTile, StructureType::Base)) {
                                    m_tileManager.placeStructure(StructureType::Base,
                                                                 tilePosition);
                                }
                            });
                            */
        }
    }
}

void ScreenGame::onUpdate(const sf::Time& dt) {}

void ScreenGame::onRender(sf::RenderWindow* window)
{
    m_camera.setViewToCamera(*window);

    // Render the tile map
    m_tileManager.showDetail = m_camera.zoomLevel < 2;
    //  m_map.draw(window);
    m_tileManager.draw(window);
    // Render the selected tile
    m_selectionRect.setTexture(&m_selectionTexture);

    m_selectionRect.setTexture(&m_selectionQuadTexture);

    if (m_tileManager.canPlaceStructure(m_selectedTile, StructureType::Base)) {
        m_selectionRect.setFillColor(sf::Color::Green);
    }
    else {
        m_selectionRect.setFillColor(sf::Color::Red);
    }

    for (int y = 0; y < 2; y++) {
        for (int x = 0; x < 2; x++) {
            m_selectionRect.setPosition(
                tileToScreenPosition(m_selectedTile - sf::Vector2i{x, y}));
            window->draw(m_selectionRect);
        }
    }
    /*
        if (m_quadDrag) {
            m_selectionRect.setTexture(&m_selectionQuadTexture);

            forEachLSection(
                m_editStartPosition, m_editPivotPoint, m_editEndPosition,
                [&](const sf::Vector2i& tilePosition) {
                    if (getStructure(StructureType::StoneWall).placement ==
                        StructurePlacement::Land) {
                        if (m_tileManager.getTile(tilePosition)->type ==
       TileType::Land) { m_selectionRect.setFillColor(sf::Color::Green);
                        }
                        else {
                            m_selectionRect.setFillColor(sf::Color::Red);
                        }
                    }
                    m_selectionRect.setPosition(tileToScreenPosition(tilePosition));

                    window->draw(m_selectionRect);
                });

            // forEachSelectedTile([&](const sf::Vector2i& tile) {
            //    m_selectionRect.setPosition(tileToScreenPosition(m_worldSize,
       tile));
            //    window->draw(m_selectionRect);
            //});
        }
        */
}
