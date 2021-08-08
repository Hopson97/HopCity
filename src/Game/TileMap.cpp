#include "TileMap.h"

#include "World.h"
#include <cmath>

namespace {
    void addIsometricQuad(std::vector<sf::Vertex>* quads,
                          const sf::Vector2i& tilePosition)
    {
        auto pos = tileToScreenPosition(tilePosition.x, tilePosition.y);

        quads->emplace_back(pos);
        quads->emplace_back(sf::Vector2f{pos.x, pos.y + TILE_HEIGHT});
        quads->emplace_back(sf::Vector2f{pos.x + TILE_WIDTH, pos.y + TILE_HEIGHT});
        quads->emplace_back(sf::Vector2f{pos.x + TILE_WIDTH, pos.y});
    }

    void addGridLine(std::vector<sf::Vertex>* gridMap, sf::Vector2f startPosition,
                     sf::Vector2f endPosition)
    {
        startPosition.x += TILE_WIDTH / 2;
        endPosition.x += TILE_WIDTH / 2;
        gridMap->emplace_back(startPosition, sf::Color::Black);
        gridMap->emplace_back(endPosition, sf::Color::Black);
    }
} // namespace

TileMap::TileMap()
    : m_tiles(WORLD_SIZE * WORLD_SIZE)
    , m_waterAnimation((unsigned)TILE_WIDTH, (unsigned)TILE_HEIGHT)
{
    m_tileTextures.loadFromFile("Data/Textures/Tilemap.png");

    for (int i = 0; i < WORLD_SIZE + 1; i++) {
        addGridLine(&m_grid, tileToScreenPosition(0, i),
                    tileToScreenPosition(WORLD_SIZE, i));
        addGridLine(&m_grid, tileToScreenPosition(i, -1),
                    tileToScreenPosition(i, WORLD_SIZE - 1));
    }

    for (int y = 0; y < WORLD_SIZE; y++) {
        for (int x = 0; x < WORLD_SIZE; x++) {
            addIsometricQuad(&m_foregroundTileVerticies, {x, y});
            addIsometricQuad(&m_backgroundTileVerticies, {x, y});
            updateTile({x, y});
        }
    }

    for (int i = 0; i < 5; i++) {
        m_waterAnimation.addFrame(0, i, sf::milliseconds(320));
    }
}

Tile* TileMap::getTile(const sf::Vector2i& position)
{
    static Tile e;
    if (position.y < 0 || position.y >= WORLD_SIZE || position.x < 0 ||
        position.x >= WORLD_SIZE) {
        return &e;
    }
    return &m_tiles.at(position.y * WORLD_SIZE + position.x);
}

void TileMap::updateTile(const sf::Vector2i& position)
{
    const sf::Vector2i TILE_OFFSETS[4] = {{0, 1}, {-1, 0}, {1, 0}, {0, -1}};

    // https://gamedevelopment.tutsplus.com/tutorials/how-to-use-tile-bitmasking-to-auto-tile-your-level-layouts--cms-25673
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

    unsigned vertexIndex = (position.y * WORLD_SIZE + position.x) * 4;
    if (vertexIndex >= m_foregroundTileVerticies.size()) {
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

void TileMap::renderTiles(sf::RenderWindow* window)
{
    sf::RenderStates state = sf::RenderStates::Default;
    state.texture = &m_tileTextures;

    auto frame = static_cast<float>(m_waterAnimation.getFrame().left);
    for (unsigned i = 0; i < m_tiles.size(); i++) {
        sf::Vertex* vertex = &m_backgroundTileVerticies[i * 4];

        vertex[0].texCoords = {frame, TILE_HEIGHT * 3};
        vertex[1].texCoords = {frame, TILE_HEIGHT * 4};
        vertex[2].texCoords = {frame + TILE_WIDTH, TILE_HEIGHT * 4};
        vertex[3].texCoords = {frame + TILE_WIDTH, TILE_HEIGHT * 3};
    }

    window->draw(m_backgroundTileVerticies.data(), m_backgroundTileVerticies.size(),
                 sf::Quads, state);
    window->draw(m_foregroundTileVerticies.data(), m_foregroundTileVerticies.size(),
                 sf::Quads, state);

    window->draw(m_grid.data(), m_grid.size(), sf::Lines);
}
