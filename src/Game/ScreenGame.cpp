#include "ScreenGame.h"
#include "ScreenMainMenu.h"
#include <cmath>
#include <imgui_sfml/imgui.h>
#include <iostream>
#include <unordered_set>

namespace {

    bool xDistGreater(const sf::Vector2i& startPoint, const sf::Vector2i& endPoint)
    {
        return std::abs(startPoint.x - endPoint.x) > std::abs(startPoint.y - endPoint.y);
    }

    struct Vec2hash {
        inline size_t operator()(const sf::Vector2i& v) const
        {
            return (v.x * 88339) ^ (v.y * 91967);
        }
    };
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
    , m_worldSize(64)
    , m_map(m_worldSize)
    , m_camera(m_worldSize)
{
    m_selectionTexture.loadFromFile("data/Textures/Selection.png");
    m_selectionRedTexture.loadFromFile("data/Textures/SelectionRed.png");
    m_tileCorners.loadFromFile("data/Textures/Corners.png");

    m_selectionRect.setSize({TILE_WIDTH, TILE_HEIGHT});
}

void ScreenGame::onInput(const Keyboard& keyboard, const sf::RenderWindow& window)
{
    auto& ts = profiler.newTimeslot("Input");
    m_camera.onInput(keyboard, window);

    auto mousePosition = sf::Mouse::getPosition(window);
    sf::Vector2f worldPos = window.mapPixelToCoords(mousePosition);

    sf::Vector2i cell = {(int)worldPos.x / (int)TILE_WIDTH,
                         (int)worldPos.y / (int)TILE_HEIGHT};
    sf::Vector2i offset = {(int)worldPos.x % (int)TILE_WIDTH,
                           (int)worldPos.y % (int)TILE_HEIGHT};

    m_selectedTile = {
        (cell.y - m_worldSize) + (cell.x - m_worldSize),
        (cell.y - m_worldSize) - (cell.x - m_worldSize),
    };

    // clang-format off
    if (offset.x >= 0 && 
        offset.y >= 0 && 
        offset.x < m_tileCorners.getSize().x && 
        offset.y < m_tileCorners.getSize().y)
    {
        sf::Color colour = m_tileCorners.getPixel(offset.x, offset.y);
             if (colour == sf::Color::Red   )   m_selectedTile.x--;
        else if (colour == sf::Color::Green )   m_selectedTile.y++;
        else if (colour == sf::Color::Blue  )   m_selectedTile.y--;
        else if (colour == sf::Color::White )   m_selectedTile.x++;
    }
    // clang-format on

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
    ts.stop();
}

void ScreenGame::onGUI()
{
    auto& ts = profiler.newTimeslot("GUI");
    if (ImGui::Begin("Info")) {
        ImGui::Text("Tile: %d %d", m_selectedTile.x, m_selectedTile.y);
        ImGuiIO& io = ImGui::GetIO();
        ImGui::Text("Performance %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate,
                    io.Framerate);
        if (ImGui::Button("Regen world")) {
            m_map.regenerateTerrain();
        }
    }
    ImGui::End();
    profiler.onGUI();
    ts.stop();
}

void ScreenGame::onEvent(const sf::Event& e)
{
    auto& ts = profiler.newTimeslot("Event");
    m_camera.onEvent(e);
    if (e.type == sf::Event::MouseButtonPressed) {
        m_quadDrag = true;
        m_editStartPosition = m_selectedTile;
        m_editEndPosition = m_selectedTile;
    }
    else if (e.type == sf::Event::MouseButtonReleased) {
        m_quadDrag = false;
        forEachLSection(
            m_editStartPosition, m_editPivotPoint, m_editEndPosition,
            [&](const sf::Vector2i& tilepos) { m_map.setTile(tilepos, TileType::Road); });
    }
    ts.stop();
}

void ScreenGame::onUpdate(const sf::Time& dt)
{
    auto& ts = profiler.newTimeslot("Update");
    ts.stop();
}

void ScreenGame::onRender(sf::RenderWindow* window)
{
    auto& ts = profiler.newTimeslot("Render");
    m_camera.setViewToCamera(*window);

    // Render the tile map
    m_map.showDetail = m_camera.zoomLevel < 2;
    m_map.draw(window);

    // Render the selected tile
    m_selectionRect.setTexture(&m_selectionTexture);
    m_selectionRect.setPosition(tileToScreenPosition(m_worldSize, m_selectedTile));
    window->draw(m_selectionRect);

    if (m_quadDrag) {
        m_selectionRect.setTexture(&m_selectionRedTexture);

        forEachLSection(m_editStartPosition, m_editPivotPoint, m_editEndPosition,
                        [&](const sf::Vector2i& tilePosition) {
                            m_selectionRect.setPosition(
                                tileToScreenPosition(m_worldSize, tilePosition));
                            window->draw(m_selectionRect);
                        });

        // forEachSelectedTile([&](const sf::Vector2i& tile) {
        //    m_selectionRect.setPosition(tileToScreenPosition(m_worldSize, tile));
        //    window->draw(m_selectionRect);
        //});
    }

    ts.stop();
}
