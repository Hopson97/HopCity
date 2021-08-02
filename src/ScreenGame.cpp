#include "ScreenGame.h"
#include "ScreenMainMenu.h"
#include <cmath>
#include <imgui_sfml/imgui.h>

ScreenGame::ScreenGame(ScreenManager* stack)
    : Screen(stack)
    , m_tiles(WORLD_SIZE * WORLD_SIZE)
{
    m_sprite.setSize({TILE_WIDTH, TILE_HEIGHT});
    m_tileTexture.loadFromFile("Data/Tiles/Grass.png");
    m_tileCorners.loadFromFile("Data/Tiles/Corners.png");
    m_roadTexture.loadFromFile("Data/Tiles/Path.png");
    m_sprite.setTexture(&m_tileTexture);

    m_view.setCenter(tileToScreenPosition(WORLD_SIZE / 2, WORLD_SIZE / 2));
    m_view.setCenter(tileToScreenPosition(0, 0));
    m_view.setSize({1600, 900});

    for (int i = 0; i < WORLD_SIZE + 1; i++) {
        // west-north grid lines
        auto startPos = tileToScreenPosition(0, i);
        auto endPos = tileToScreenPosition(WORLD_SIZE, i);
        startPos.x += TILE_WIDTH / 2;
        endPos.x += TILE_WIDTH / 2;
        m_grid.emplace_back(startPos, sf::Color::Black);
        m_grid.emplace_back(endPos, sf::Color::Black);

        // east-west grid lines
        startPos = tileToScreenPosition(i, -1);
        endPos = tileToScreenPosition(i, WORLD_SIZE - 1);
        startPos.y += TILE_HEIGHT / 2;
        endPos.y += TILE_HEIGHT / 2;
        m_grid.emplace_back(startPos, sf::Color::Black);
        m_grid.emplace_back(endPos, sf::Color::Black);
    }
}

void ScreenGame::onInput(const Keyboard& keyboard, const sf::RenderWindow& window)
{

    // Move the view if it is on the edge of the screen
    constexpr int GAP = 100;
    auto mousePosition = sf::Mouse::getPosition(window);
    if (mousePosition.x < GAP) {
        m_view.move(-5, 0);
    }
    else if (mousePosition.x > (int)window.getSize().x - GAP) {
        m_view.move(5, 0);
    }
    if (mousePosition.y < GAP) {
        m_view.move(0, -5);
    }
    else if (mousePosition.y > (int)window.getSize().y - GAP) {
        m_view.move(0, 5);
    }

    if (keyboard.isKeyDown(sf::Keyboard::A)) {
        m_view.move(-4, 0);
    }
    else if (keyboard.isKeyDown(sf::Keyboard::D)) {
        m_view.move(4, 0);
    }

    if (keyboard.isKeyDown(sf::Keyboard::S)) {
        m_view.move(0, 4);
    }
    else if (keyboard.isKeyDown(sf::Keyboard::W)) {
        m_view.move(0, -4);
    }

    sf::Vector2f worldPos = window.mapPixelToCoords(mousePosition);

    sf::Vector2i cell = {(int)worldPos.x / (int)TILE_WIDTH,
                         (int)worldPos.y / (int)TILE_HEIGHT};
    sf::Vector2i offset = {(int)worldPos.x % (int)TILE_WIDTH,
                           (int)worldPos.y % (int)TILE_HEIGHT};

    m_selectedTile = {
        (cell.y - (int)m_originOffset.y) + (cell.x - (int)m_originOffset.x),
        (cell.y - (int)m_originOffset.y) - (cell.x - (int)m_originOffset.x),
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

void ScreenGame::onGUI() {}

Tile* ScreenGame::getTile(const sf::Vector2i& position)
{
    return &m_tiles.at(position.y * WORLD_SIZE + position.x);
}

void ScreenGame::updateTileVarient([[maybe_unused]] const sf::Vector2i& position) {}

void ScreenGame::onEvent(const sf::Event& e)
{
    if (e.type == sf::Event::MouseButtonReleased) {
        Tile* tile = &m_tiles.at(m_selectedTile.y * WORLD_SIZE + m_selectedTile.x);
        tile->varient = 0;
        if (tile->type == TileType::Grass) {
            tile->type = TileType::Road;
            sf::Vector2i offsets[4] = {{0, 1}, {-1, 0}, {1, 0}, {0, -1}};

            // Update the road tiles to be the correct varient
            // https://gamedevelopment.tutsplus.com/tutorials/how-to-use-tile-bitmasking-to-auto-tile-your-level-layouts--cms-25673
            for (int i = 0; i < 4; i++) {
                Tile* neighbour = getTile(m_selectedTile + offsets[i]);
                if (neighbour->type == tile->type) {

                    // Cycle through 1, 2, 4, 8 or 2^0, 2^1, 2^2, 2^3
                    tile->varient += std::pow(2, i);

                    // Also update neighbour road varient
                    neighbour->varient = 0;
                    for (int j = 0; j < 4; j++) {
                        neighbour->varient +=
                            std::pow(2, j) *
                            (getTile(m_selectedTile + offsets[i] + offsets[j])->type ==
                             tile->type);
                    }
                }
            }
        }
        else if (tile->type == TileType::Road) {
            tile->type = TileType::Grass;
            sf::Vector2i offsets[4] = {{0, 1}, {-1, 0}, {1, 0}, {0, -1}};
            for (int i = 0; i < 4; i++) {
                Tile* neighbour = getTile(m_selectedTile + offsets[i]);
                if (neighbour->type == TileType::Road) {
                    neighbour->varient = 0;
                    for (int j = 0; j < 4; j++) {
                        neighbour->varient +=
                            std::pow(2, j) *
                            (getTile(m_selectedTile + offsets[i] + offsets[j])->type ==
                             neighbour->type);
                    }
                }
            }
        }
    }
}

void ScreenGame::onUpdate(const sf::Time& dt) {}

sf::Vector2f ScreenGame::tileToScreenPosition(int x, int y)
{
    return {(m_originOffset.x * TILE_WIDTH) + (x - y) * (TILE_WIDTH / 2.0f),
            (m_originOffset.y * TILE_HEIGHT) + (x + y) * (TILE_HEIGHT / 2.0f)};
}

void ScreenGame::onRender(sf::RenderWindow* window)
{
    window->setView(m_view);

    for (int y = 0; y < WORLD_SIZE; y++) {
        for (int x = 0; x < WORLD_SIZE; x++) {
            const Tile& tile = m_tiles[y * WORLD_SIZE + x];
            m_sprite.setTextureRect(sf::IntRect{tile.varient * (int)TILE_WIDTH, 0,
                                                (int)TILE_WIDTH, (int)TILE_HEIGHT});
            switch (tile.type) {
                case TileType::Grass:
                    m_sprite.setTexture(&m_tileTexture);
                    break;

                case TileType::Road:
                    m_sprite.setTexture(&m_roadTexture);
                    break;
            }

            auto pos = tileToScreenPosition(x, y);
            m_sprite.setPosition(pos);

            window->draw(m_sprite);
        }
    }

    m_sprite.setPosition(tileToScreenPosition(m_selectedTile.x, m_selectedTile.y));
    m_sprite.setFillColor(sf::Color::Red);
    window->draw(m_sprite);
    m_sprite.setFillColor(sf::Color::White);

    window->draw(m_grid.data(), m_grid.size(), sf::Lines);

    if (ImGui::Begin("Info")) {
        ImGui::Text("Tile: %d %d", m_selectedTile.x, m_selectedTile.y);
    }
    ImGui::End();
}
