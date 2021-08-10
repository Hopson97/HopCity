#include "Map.h"

#include "World.h"
#include <cmath>

namespace {
    void addIsometricQuad(std::vector<sf::Vertex>* quads, int worldSize,
                          const sf::Vector2i& tilePosition)
    {
        auto pos = tileToScreenPosition(worldSize, tilePosition);

        quads->emplace_back(pos);
        quads->emplace_back(sf::Vector2f{pos.x, pos.y + TILE_HEIGHT});
        quads->emplace_back(sf::Vector2f{pos.x + TILE_WIDTH, pos.y + TILE_HEIGHT});
        quads->emplace_back(sf::Vector2f{pos.x + TILE_WIDTH, pos.y});
    }

    void addGridLine(std::vector<sf::Vertex>* gridMap, sf::Vector2f startPosition,
                     sf::Vector2f endPosition)
    {
        sf::Color gridColour = {0, 0, 0, 100};

        startPosition.x += TILE_WIDTH / 2;
        endPosition.x += TILE_WIDTH / 2;
        gridMap->emplace_back(startPosition, gridColour);
        gridMap->emplace_back(endPosition, gridColour);
    }
} // namespace

Map::Map(int worldSize)
    : m_waterAnimation((int)TILE_WIDTH, (int)TILE_HEIGHT, true, 0.05)
    , m_worldSize(worldSize)
{
    m_tileTextures.loadFromFile("Data/Textures/TileMap.png");
    regenerate();

}

void Map::setTile(const sf::Vector2i& position, TileType type)
{
    getTile(position)->type = type;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            updateTile(position + sf::Vector2i{i, j});
        }
    }
}

Tile* Map::getTile(const sf::Vector2i& position)
{
    static Tile e;
    if (position.y < 0 || position.y >= m_worldSize || position.x < 0 ||
        position.x >= m_worldSize) {
        return &e;
    }
    return &m_tiles.at(position.y * m_worldSize + position.x);
}

void Map::updateTile(const sf::Vector2i& position)
{
    const sf::Vector2i TILE_OFFSETS[4] = {{0, 1}, {-1, 0}, {1, 0}, {0, -1}};

    // https://gamedevelopment.tutsplus.com/tutorials/how-to-use-tile-bitmasking-to-auto-tile-your-level-layouts--cms-25673

    // Set the tile's varient
    Tile* tile = getTile(position);
    tile->varient = 0;
    for (int i = 0; i < 4; i++) {
        Tile* neighbour = getTile(position + TILE_OFFSETS[i]);
        if (neighbour && tile->type == neighbour->type) {
            tile->varient += (int)std::pow(2, i);
        }
        if (neighbour && neighbour->type != TileType::Grass) {
            neighbour->varient = 0;
            for (int j = 0; j < 4; j++) {
                Tile* subNeighbour =
                    getTile(position + TILE_OFFSETS[i] + TILE_OFFSETS[j]);
                if (subNeighbour && subNeighbour->type == neighbour->type) {
                    neighbour->varient += (int)std::pow(2, j);
                }
            }
        }
    }

    // Update the tile texture coords
    int vertexIndex = (position.y * m_worldSize + position.x) * 4;
    if (vertexIndex >= (int)m_foregroundTileVerticies.size()) {
        return;
    }
    sf::Vertex* vertex = &m_foregroundTileVerticies[vertexIndex];

    if (tile->type == TileType::Grass) {
        vertex[0].texCoords = {0, 0};
        vertex[1].texCoords = {0, TILE_HEIGHT};
        vertex[2].texCoords = {TILE_WIDTH, TILE_HEIGHT};
        vertex[3].texCoords = {TILE_WIDTH, 0};
    }
    else if (tile->type == TileType::Road) {
        vertex[0].texCoords = {tile->varient * TILE_WIDTH, TILE_HEIGHT};
        vertex[1].texCoords = {tile->varient * TILE_WIDTH, TILE_HEIGHT * 2.0f};
        vertex[2].texCoords = {tile->varient * TILE_WIDTH + TILE_WIDTH,
                               TILE_HEIGHT * 2.0f};
        vertex[3].texCoords = {tile->varient * TILE_WIDTH + TILE_WIDTH, TILE_HEIGHT};
    }
    else if (tile->type == TileType::Water) {
        vertex[0].texCoords = {tile->varient * TILE_WIDTH, TILE_HEIGHT * 2.0f};
        vertex[1].texCoords = {tile->varient * TILE_WIDTH, TILE_HEIGHT * 3.0f};
        vertex[2].texCoords = {tile->varient * TILE_WIDTH + TILE_WIDTH,
                               TILE_HEIGHT * 3.0f};
        vertex[3].texCoords = {tile->varient * TILE_WIDTH + TILE_WIDTH,
                               TILE_HEIGHT * 2.0f};
    }
}

void Map::draw(sf::RenderWindow* target)
{
    sf::RenderStates states = sf::RenderStates::Default;
    states.texture = &m_tileTextures;

    if (showDetail) {
        auto frame = static_cast<float>(m_waterAnimation.getFrame().left);
        for (unsigned i = 0; i < m_tiles.size(); i++) {
            sf::Vertex* vertex = &m_backgroundTileVerticies[i * 4];

            vertex[0].texCoords = {frame, TILE_HEIGHT * 3};
            vertex[1].texCoords = {frame, TILE_HEIGHT * 4};
            vertex[2].texCoords = {frame + TILE_WIDTH, TILE_HEIGHT * 4};
            vertex[3].texCoords = {frame + TILE_WIDTH, TILE_HEIGHT * 3};
        }
    }
    target->draw(m_backgroundTileVerticies.data(), m_backgroundTileVerticies.size(),
                 sf::Quads, states);
    target->draw(m_foregroundTileVerticies.data(), m_foregroundTileVerticies.size(),
                 sf::Quads, states);

    if (showDetail) {
        target->draw(m_grid.data(), m_grid.size(), sf::Lines);
    }
}
void Map::regenerate() {
    m_tiles = generateWorld({0, 0}, m_worldSize);

    for (int i = 0; i < m_worldSize + 1; i++) {
        addGridLine(&m_grid, tileToScreenPosition(m_worldSize, {0, i}),
                    tileToScreenPosition(m_worldSize, {m_worldSize, i}));
        addGridLine(&m_grid, tileToScreenPosition(m_worldSize, {i, 0}),
                    tileToScreenPosition(m_worldSize, {i, m_worldSize}));
    }

    for (int y = 0; y < m_worldSize; y++) {
        for (int x = 0; x < m_worldSize; x++) {
            addIsometricQuad(&m_foregroundTileVerticies, m_worldSize, {x, y});
            addIsometricQuad(&m_backgroundTileVerticies, m_worldSize, {x, y});
            updateTile({x, y});
        }
    }

    for (int i = 0; i < 5; i++) {
        m_waterAnimation.addFrame(0, i, sf::milliseconds(320));
    }
}
void Map::onGUI() {
    if(ImGui::Begin("World Generation")){
        ImGui::TextColored({0.960, 0.815, 0.360,1},"Noise Options");
        ImGui::SliderFloat("Amplitude",&terrainGenOptions.amplitude,0.f,1.f);
        ImGui::SliderFloat("Amplitude",&terrainGenOptions.frequency,0.f,1.f);
        ImGui::SliderInt("Octaves",&terrainGenOptions.octaves,0,10);
        if(ImGui::Button("Regenerate World")){
            regenerate();
        }
    }
    ImGui::End();
}
