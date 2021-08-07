#include "TileMap.h"

#include "World.h"

TileMap::TileMap()
    : m_tiles(WORLD_SIZE * WORLD_SIZE)
    , m_wateranim((unsigned)TILE_WIDTH, (unsigned)TILE_HEIGHT)
{
    m_tilemap.loadFromFile("Data/Textures/Tilemap.png");

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

            updateTile({x, y});
        }
    }

    m_wateranim.addFrame(0, 0, sf::milliseconds(320));
    m_wateranim.addFrame(0, 1, sf::milliseconds(320));
    m_wateranim.addFrame(0, 2, sf::milliseconds(320));
    m_wateranim.addFrame(0, 3, sf::milliseconds(320));
    m_wateranim.addFrame(0, 4, sf::milliseconds(320));
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
    if (vertexIndex >= m_tileVerts.size()) {
        return;
    }
    sf::Vertex* v = &m_tileVerts[vertexIndex];

    if (tile->type == TileType::Grass) {
        v[0].texCoords = {0, 0};
        v[1].texCoords = {0, TILE_HEIGHT};
        v[2].texCoords = {TILE_WIDTH, TILE_HEIGHT};
        v[3].texCoords = {TILE_WIDTH, 0};
    }
    else if (tile->type == TileType::Road) {
        v[0].texCoords = {tile->varient * TILE_WIDTH, TILE_HEIGHT};
        v[1].texCoords = {tile->varient * TILE_WIDTH, TILE_HEIGHT * 2.0f};
        v[2].texCoords = {tile->varient * TILE_WIDTH + TILE_WIDTH, TILE_HEIGHT * 2.0f};
        v[3].texCoords = {tile->varient * TILE_WIDTH + TILE_WIDTH, TILE_HEIGHT};
    }
    else if (tile->type == TileType::Water) {
        v[0].texCoords = {tile->varient * TILE_WIDTH, TILE_HEIGHT * 2.0f};
        v[1].texCoords = {tile->varient * TILE_WIDTH, TILE_HEIGHT * 3.0f};
        v[2].texCoords = {tile->varient * TILE_WIDTH + TILE_WIDTH, TILE_HEIGHT * 3.0f};
        v[3].texCoords = {tile->varient * TILE_WIDTH + TILE_WIDTH, TILE_HEIGHT * 2.0f};
    }
}

void TileMap::renderTiles(sf::RenderWindow* window) 
{
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

    window->draw(m_grid.data(), m_grid.size(), sf::Lines);


}
