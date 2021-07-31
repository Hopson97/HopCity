#include "ScreenGame.h"
#include "ScreenMainMenu.h"
#include <cmath>
#include <imgui_sfml/imgui.h>

ScreenGame::ScreenGame(ScreenManager* stack)
    : Screen(stack)
{
    m_sprite.setSize({TILE_WIDTH, TILE_HEIGHT});
    m_tileTexture.loadFromFile("Data/Tiles/Grass.png");
    m_sprite.setTexture(&m_tileTexture);

    m_view.setCenter(0, 0);
    m_view.setSize({1280, 720});
}

void ScreenGame::onGUI() {}

void ScreenGame::onEvent(const sf::Event& e)
{

}


void ScreenGame::onUpdate(const sf::Time& dt) {}

sf::Vector2f tileToScreenPosition(int x, int y)
{
    return {(x - y) * (TILE_WIDTH / 2.0f),
            (x + y) * (TILE_HEIGHT / 2.0f)};
}

void ScreenGame::onRender(sf::RenderWindow* window)
{
    window->setView(m_view);
    sf::Vector2f worldPos = window->mapPixelToCoords(sf::Mouse::getPosition(*window));

    sf::Vector2i cell = {
        (int)worldPos.x / (int)TILE_WIDTH, (int)worldPos.y / (int)TILE_HEIGHT
    };
    sf::Vector2i offset = {
        (int)worldPos.x % (int)TILE_WIDTH, (int)worldPos.y % (int)TILE_HEIGHT
    };

    sf::Vector2i selected = {
        cell.y + cell.x,
        cell.y - cell.x,
    };

    


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
}
