#include "ScreenGame.h"
#include "ScreenMainMenu.h"
#include <cmath>
#include <imgui_sfml/imgui.h>
#include <iostream>

const sf::Vector2i TILE_OFFSETS[4] = {{0, 1}, {-1, 0}, {1, 0}, {0, -1}};

ScreenGame::ScreenGame(ScreenManager* stack)
    : Screen(stack)
    , m_tiles(WORLD_SIZE * WORLD_SIZE)
    , m_wateranim(TILE_WIDTH, TILE_HEIGHT)
{
    m_tilemap.loadFromFile("Data/Textures/Tilemap.png");
    m_selectionTexture.loadFromFile("Data/Textures/Selection.png");
    m_tileCorners.loadFromFile("Data/Textures/Corners.png");

    m_tileRect.setTexture(&m_selectionTexture);
    m_tileRect.setSize({TILE_WIDTH, TILE_HEIGHT});

    m_view.setCenter(tileToScreenPosition(WORLD_SIZE / 2, WORLD_SIZE / 2));
    m_view.setSize({1600, 900});
    m_view.zoom(m_currentZoom);

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
    m_wateranim.addFrame(0, 0, sf::milliseconds(320));
    m_wateranim.addFrame(0, 1, sf::milliseconds(320));
    m_wateranim.addFrame(0, 2, sf::milliseconds(320));
    m_wateranim.addFrame(0, 3, sf::milliseconds(320));
    m_wateranim.addFrame(0, 4, sf::milliseconds(320));

    // Generate """islands"""
    for (int y = 25; y < WORLD_SIZE - 25; y++) {
        for (int x = 25; x < WORLD_SIZE - 25; x++) {
            if (rand() % 1000 > 998) {
                getTile({x, y})->type = TileType::Grass;

                int w = rand() % 10;
                for (int iy = 0; iy < w; iy++) {
                    for (int ix = 0; ix < w; ix++) {
                        getTile({x + ix, y + iy})->type = TileType::Grass;
                    }
                }
            }
        }
    }

    // "Auto tile" the tilemap
    for (int y = 0; y < WORLD_SIZE; y++) {
        for (int x = 0; x < WORLD_SIZE; x++) {
            auto pos = tileToScreenPosition(x, y);

            m_tileVerts.emplace_back(pos);
            m_tileVerts.emplace_back(sf::Vector2f{pos.x, pos.y + TILE_HEIGHT});
            m_tileVerts.emplace_back(
                sf::Vector2f{pos.x + TILE_WIDTH, pos.y + TILE_HEIGHT});
            m_tileVerts.emplace_back(sf::Vector2f{pos.x + TILE_WIDTH, pos.y});

            m_waterAnimationVerts.emplace_back(pos);
            m_waterAnimationVerts.emplace_back(sf::Vector2f{pos.x, pos.y + TILE_HEIGHT});
            m_waterAnimationVerts.emplace_back(
                sf::Vector2f{pos.x + TILE_WIDTH, pos.y + TILE_HEIGHT});
            m_waterAnimationVerts.emplace_back(sf::Vector2f{pos.x + TILE_WIDTH, pos.y});

            autoTile({x, y});
            updateTileTextureCoords({x, y});
        }
    }
}

void ScreenGame::updateTileTextureCoords(const sf::Vector2i& position)
{
    sf::Vertex* v = &m_tileVerts[(position.y * WORLD_SIZE + position.x) * 4];
    const Tile* tile = getTile(position);

    if (tile->type == TileType::Grass) {
        v[0].texCoords = {0, 0};
        v[1].texCoords = {0, TILE_HEIGHT};
        v[2].texCoords = {TILE_WIDTH, TILE_HEIGHT};
        v[3].texCoords = {TILE_WIDTH, 0};
    }
    else if (tile->type == TileType::Road) {
        v[0].texCoords = {tile->varient * (int)TILE_WIDTH, TILE_HEIGHT};
        v[1].texCoords = {tile->varient * (int)TILE_WIDTH, TILE_HEIGHT * 2.0f};
        v[2].texCoords = {tile->varient * (int)TILE_WIDTH + TILE_WIDTH,
                          TILE_HEIGHT * 2.0f};
        v[3].texCoords = {tile->varient * (int)TILE_WIDTH + TILE_WIDTH, TILE_HEIGHT};
    }
    else if (tile->type == TileType::Water) {
        v[0].texCoords = {tile->varient * (int)TILE_WIDTH, TILE_HEIGHT * 2.0f};
        v[1].texCoords = {tile->varient * (int)TILE_WIDTH, TILE_HEIGHT * 3.0f};
        v[2].texCoords = {tile->varient * (int)TILE_WIDTH + TILE_WIDTH,
                          TILE_HEIGHT * 3.0f};
        v[3].texCoords = {tile->varient * (int)TILE_WIDTH + TILE_WIDTH,
                          TILE_HEIGHT * 2.0f};
    }
}

void ScreenGame::onInput(const Keyboard& keyboard, const sf::RenderWindow& window)
{

    // Move the view if it is on the edge of the screen
    constexpr int GAP = 100;
    auto mousePosition = sf::Mouse::getPosition(window);
    if (mousePosition.x < GAP) {
        //    m_view.move(-5, 0);
    }
    else if (mousePosition.x > (int)window.getSize().x - GAP) {
        //   m_view.move(5, 0);
    }
    if (mousePosition.y < GAP) {
        //     m_view.move(0, -5);
    }
    else if (mousePosition.y > (int)window.getSize().y - GAP) {
        //      m_view.move(0, 5);
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
    static Tile e;
    if (position.y < 0 || position.y >= WORLD_SIZE || position.x < 0 ||
        position.x >= WORLD_SIZE) {
        return &e;
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
    else if (e.type == sf::Event::MouseWheelScrolled) {
        if (e.mouseWheelScroll.delta > 0 && m_currentZoom == 8) {
            m_view.zoom(1.0f / 8.0f);
            m_currentZoom *= 1.0f / 8.0f;
        }
        else if (e.mouseWheelScroll.delta > 0 && m_currentZoom > 0.065) {
            m_view.zoom(0.5f);
            m_currentZoom *= 0.5f;
        }
        else if (e.mouseWheelScroll.delta < 0 && m_currentZoom < 1) {
            m_view.zoom(2.0f);
            m_currentZoom *= 2.0f;
        }
        else if (e.mouseWheelScroll.delta < 0 && (int)m_currentZoom == 1) {
            m_view.zoom(8.0f);
            m_currentZoom = 8.0f;
        }

        std::cout << m_currentZoom << std::endl;
    }

    if (m_mousedown) {
        Tile* tile = &m_tiles.at(m_selectedTile.y * WORLD_SIZE + m_selectedTile.x);
        if (!tile) {
            return;
        }
        tile->varient = 0;
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

        autoTile(m_selectedTile);
        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                updateTileTextureCoords(m_selectedTile + sf::Vector2i{i, j});
            }
        }
    }
}

void ScreenGame::autoTile(const sf::Vector2i& position)
{
    https://gamedevelopment.tutsplus.com/tutorials/how-to-use-tile-bitmasking-to-auto-tile-your-level-layouts--cms-25673
    for (int i = 0; i < 4; i++) {
        Tile* neighbour = getTile(position + TILE_OFFSETS[i]);
        if (neighbour && neighbour->type != TileType::Grass) {
            neighbour->varient = 0;
            for (int j = 0; j < 4; j++) {
                Tile* subNeighbour =
                    getTile(position + TILE_OFFSETS[i] + TILE_OFFSETS[j]);
                if (subNeighbour && subNeighbour->type == neighbour->type) {
                    neighbour->varient += std::pow(2, j);
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

    // Render the tile map
    sf::RenderStates state = sf::RenderStates::Default;
    state.texture = &m_tilemap;

    auto frame = static_cast<float>(m_wateranim.getFrame().left);
    for (unsigned i = 0; i < m_tiles.size(); i++) {
        sf::Vertex* v = &m_waterAnimationVerts[i * 4];

        v[0].texCoords = {frame, TILE_HEIGHT * 3};
        v[1].texCoords = {frame, TILE_HEIGHT * 4};
        v[2].texCoords = {frame + TILE_WIDTH, TILE_HEIGHT * 4};
        v[3].texCoords = {frame + TILE_WIDTH, TILE_HEIGHT * 3};
    }

    window->draw(m_waterAnimationVerts.data(), m_waterAnimationVerts.size(), sf::Quads,
                 state);
    window->draw(m_tileVerts.data(), m_tileVerts.size(), sf::Quads, state);

    // Render the selected tile
    m_tileRect.setPosition(tileToScreenPosition(m_selectedTile.x, m_selectedTile.y));
    window->draw(m_tileRect);

    // Render the the tile map grid
    if (drawGrid && (int)m_currentZoom != 8) {
        window->draw(m_grid.data(), m_grid.size(), sf::Lines);
    }

    if (ImGui::Begin("Info")) {
        ImGui::Text("Tile: %d %d", m_selectedTile.x, m_selectedTile.y);
        ImGui::Checkbox("Draw Grid", &drawGrid);
    }
    ImGui::End();
    ImGui::ShowMetricsWindow(nullptr);
}
