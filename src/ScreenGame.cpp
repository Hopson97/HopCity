#include "ScreenGame.h"
#include "ScreenMainMenu.h"
#include <cmath>
#include <imgui_sfml/imgui.h>

ScreenGame::ScreenGame(ScreenManager* stack)
    : Screen(stack)
{
    m_sprite.setSize({TILE_WIDTH, TILE_HEIGHT});
    m_tileTexture.loadFromFile("Data/Tiles/Grass.png");
    m_tileCorners.loadFromFile("Data/Tiles/Corners.png");
    m_sprite.setTexture(&m_tileTexture);

    m_view.setCenter(tileToScreenPosition(WORLD_SIZE / 2, WORLD_SIZE / 2));
    m_view.setCenter(tileToScreenPosition(0, 0));
    m_view.setSize({1600, 900});

    for (int i = 0; i < WORLD_SIZE + 1; i++) {
        // west-north grid lines
        auto startPos = tileToScreenPosition(0, i);
        auto endPos = tileToScreenPosition(WORLD_SIZE, i);
        startPos.x += TILE_WIDTH/ 2;
        endPos.x += TILE_WIDTH/ 2;
        m_grid.emplace_back(startPos, sf::Color::Black);
        m_grid.emplace_back(endPos, sf::Color::Black);

        // east-west grid lines
        startPos = tileToScreenPosition(i, -1);
        endPos = tileToScreenPosition(i, WORLD_SIZE - 1);
        startPos.y += TILE_HEIGHT/ 2;
        endPos.y += TILE_HEIGHT / 2;
        m_grid.emplace_back(startPos, sf::Color::Black);
        m_grid.emplace_back(endPos, sf::Color::Black);

    }
}

void ScreenGame::onGUI() {}

void ScreenGame::onEvent(const sf::Event& e) {}

void ScreenGame::onUpdate(const sf::Time& dt) {}

sf::Vector2f ScreenGame::tileToScreenPosition(int x, int y)
{
    static bool o = false;
    if (!o) {
        std::printf("x %f y %f",
                    (m_originOffset.x * TILE_WIDTH) + (x - y) * (TILE_WIDTH / 2.0f),
                    (m_originOffset.y * TILE_HEIGHT) + (x + y) * (TILE_HEIGHT / 2.0f));
        o = true;
    }
    return {(m_originOffset.x * TILE_WIDTH) + (x - y) * (TILE_WIDTH / 2.0f),
            (m_originOffset.y * TILE_HEIGHT) + (x + y) * (TILE_HEIGHT / 2.0f)};
}

void ScreenGame::onRender(sf::RenderWindow* window)
{
    window->setView(m_view);
    sf::Vector2f worldPos = window->mapPixelToCoords(sf::Mouse::getPosition(*window));

    sf::Vector2i cell = {(int)worldPos.x / (int)TILE_WIDTH,
                         (int)worldPos.y / (int)TILE_HEIGHT};
    sf::Vector2i offset = {(int)worldPos.x % (int)TILE_WIDTH,
                           (int)worldPos.y % (int)TILE_HEIGHT};

    sf::Vector2i selected = {
        (cell.y - (int)m_originOffset.y) + (cell.x - (int)m_originOffset.x),
        (cell.y - (int)m_originOffset.y) - (cell.x - (int)m_originOffset.x),
    };

    sf::Color colour = m_tileCorners.getPixel(offset.x, offset.y);
    if (colour == sf::Color::Red)
        selected.x--;
    else if (colour == sf::Color::Green)
        selected.y++;
    else if (colour == sf::Color::Blue)
        selected.y--;
    else if (colour == sf::Color::White)
        selected.x++;

    for (int y = 0; y < WORLD_SIZE; y++) {
        for (int x = 0; x < WORLD_SIZE; x++) {
            auto pos = tileToScreenPosition(x, y);
            m_sprite.setPosition(pos);

            window->draw(m_sprite);
        }
    }

    m_sprite.setPosition(tileToScreenPosition(selected.x, selected.y));
    m_sprite.setOutlineThickness(1);
    m_sprite.setFillColor(sf::Color::Blue);
    window->draw(m_sprite);
    m_sprite.setFillColor(sf::Color::White);
    m_sprite.setOutlineThickness(0);

    if (ImGui::Begin("Info")) {
        ImGui::Text("Mouse: %f %f", worldPos.x, worldPos.y);
        ImGui::Text("Cell: %d %d", cell.x, cell.y);
        ImGui::Text("Tile: %d %d", selected.x, selected.y);
    }
    ImGui::End();

    window->draw(m_grid.data(), m_grid.size(), sf::Lines);
}
