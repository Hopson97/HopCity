#include "ScreenGame.h"
#include "ScreenMainMenu.h"
#include <cmath>
#include <imgui_sfml/imgui.h>

const sf::Vector2i TILE_OFFSETS[4] = {{0, 1}, {-1, 0}, {1, 0}, {0, -1}};

ScreenGame::ScreenGame(ScreenManager* stack)
    : Screen(stack)
    , m_tiles(WORLD_SIZE * WORLD_SIZE)
{
    m_tilemap.loadFromFile("Data/Tiles.png");
    m_tileRect.setTexture(&m_tilemap);
    m_tileRect.setSize({TILE_WIDTH, TILE_HEIGHT});
    m_tileCorners.loadFromFile("Data/Tiles/Corners.png");

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
    if (position.y < 0 || position.y >= WORLD_SIZE || position.x < 0 ||
        position.x >= WORLD_SIZE) {
        return nullptr;
    }
    return &m_tiles.at(position.y * WORLD_SIZE + position.x);
}

void ScreenGame::onEvent(const sf::Event& e)
{
    if (e.type == sf::Event::MouseButtonPressed) {
        m_mousedown = true;
        m_buttonPressed = e.mouseButton.button;
    }
    else if (e.type == sf::Event::MouseButtonReleased) {
        m_mousedown = false;
    }

    if (m_mousedown) {
        Tile* tile = &m_tiles.at(m_selectedTile.y * WORLD_SIZE + m_selectedTile.x);
        if (!tile) {
            return;
        }
        tile->varient = 0;
        if (m_buttonPressed == sf::Mouse::Left) {
            tile->type = TileType::Road;

            // Update the road tiles to be the correct varient
            // https://gamedevelopment.tutsplus.com/tutorials/how-to-use-tile-bitmasking-to-auto-tile-your-level-layouts--cms-25673
            for (int i = 0; i < 4; i++) {
                Tile* neighbour = getTile(m_selectedTile + TILE_OFFSETS[i]);
                if (neighbour && neighbour->type == tile->type) {

                    // Cycle through 1, 2, 4, 8 or 2^0, 2^1, 2^2, 2^3
                    tile->varient += std::pow(2, i);

                    // Also update neighbour road varient
                    neighbour->varient = 0;
                    for (int j = 0; j < 4; j++) {
                        Tile* subNeighbour =
                            getTile(m_selectedTile + TILE_OFFSETS[i] + TILE_OFFSETS[j]);
                        if (subNeighbour && subNeighbour->type == tile->type) {
                            neighbour->varient += std::pow(2, j);
                        }
                    }
                }
            }
        }
        else if (m_buttonPressed == sf::Mouse::Right) {
            tile->type = TileType::Grass;
            for (int i = 0; i < 4; i++) {
                Tile* neighbour = getTile(m_selectedTile + TILE_OFFSETS[i]);
                if (neighbour && neighbour->type == TileType::Road) {
                    neighbour->varient = 0;
                    for (int j = 0; j < 4; j++) {
                        Tile* subNeighbour =
                            getTile(m_selectedTile + TILE_OFFSETS[i] + TILE_OFFSETS[j]);
                        if (subNeighbour && subNeighbour->type == neighbour->type) {
                            neighbour->varient += std::pow(2, j);
                        }
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
            const Tile* tile = getTile({x, y});
            auto pos = tileToScreenPosition(x, y);

            auto textureY = tile->type == TileType::Grass ? 0 : TILE_HEIGHT;
            m_tileRect.setTextureRect(sf::IntRect{tile->varient * (int)TILE_WIDTH,
                                                  textureY, (int)TILE_WIDTH,
                                                  (int)TILE_HEIGHT});
            m_tileRect.setPosition(pos);

            window->draw(m_tileRect);
        }
    }

    m_tileRect.setPosition(tileToScreenPosition(m_selectedTile.x, m_selectedTile.y));
    m_tileRect.setFillColor(sf::Color::Red);
    window->draw(m_tileRect);
    m_tileRect.setFillColor(sf::Color::White);

    if (drawGrid) {
        window->draw(m_grid.data(), m_grid.size(), sf::Lines);
    }
    if (ImGui::Begin("Info")) {
        ImGui::Text("Tile: %d %d", m_selectedTile.x, m_selectedTile.y);
        ImGui::Checkbox("Draw Grid", &drawGrid);
    }
    ImGui::End();
}
