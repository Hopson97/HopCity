#include "Map.h"

#include "WorldGeneration.h"
#include <cmath>
#include <iostream>
#include <random>

#include "Common.h"
#include <SFML/Window/Mouse.hpp>

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

    // https://gamedevelopment.tutsplus.com/tutorials/how-to-use-tile-bitmasking-to-auto-tile-your-level-layouts--cms-25673
    const sf::Vector2i TILE_OFFSETS[4] = {{0, 1}, {-1, 0}, {1, 0}, {0, -1}};

} // namespace

/*
Map::Map(int worldSize)
    : m_worldSize(worldSize)
{
    m_tileTextures.loadFromFile("data/Textures/TileMap2.png");
    m_structureMap.loadFromFile("data/Textures/Structures.png");

    m_structureRect.setTexture(&m_structureMap);

    for (int i = 0; i < m_worldSize + 1; i++) {
        addGridLine(&m_grid, tileToScreenPosition(worldSize, {0, i}),
                    tileToScreenPosition(worldSize, {m_worldSize, i}));
        addGridLine(&m_grid, tileToScreenPosition(worldSize, {i, 0}),
                    tileToScreenPosition(worldSize, {i, m_worldSize}));
    }
}

void Map::initWorld()
{
    m_tiles = generateWorld({0, 0}, m_worldSize, this);

    for (int y = 0; y < m_worldSize; y++) {
        for (int x = 0; x < m_worldSize; x++) {
            addIsometricQuad(&m_tileVerts, m_worldSize, {x, y});
            updateTile({x, y});
        }
    }
}

void Map::regenerateTerrain()
{
    m_structures.clear();
    sorted.clear();
    m_tiles = generateWorld({0, 0}, m_worldSize, this);

    for (int y = 0; y < m_worldSize; y++) {
        for (int x = 0; x < m_worldSize; x++) {
            updateTile({x, y});
        }
    }
}

void Map::setTile(const sf::Vector2i& position, TileType type)
{
    // Check for out of bounds
    if (position.y < 0 || position.y >= m_worldSize || position.x < 0 ||
        position.x >= m_worldSize) {
        return;
    }

    // Set tile + update its neighbours
    getTile(position)->type = type;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            updateTile(position + sf::Vector2i{i, j});
        }
    }
}

Tile* Map::getTile(const sf::Vector2i& position)
{
    // Check for out of bounds
    static Tile e;
    if (position.y < 0 || position.y >= m_worldSize || position.x < 0 ||
        position.x >= m_worldSize) {
        return &e;
    }

    // Get the tile
    return &m_tiles.at(position.y * m_worldSize + position.x);
}

// Used for the "bitmask" techique to auto tile structures and the land
//
https://gamedevelopment.tutsplus.com/tutorials/how-to-use-tile-bitmasking-to-auto-tile-your-level-layouts--cms-25673
const sf::Vector2i TILE_OFFSETS[4] = {{0, 1}, {-1, 0}, {1, 0}, {0, -1}};

void Map::updateTile(const sf::Vector2i& position)
{
    // Set the tile'structure variant
    Tile* tile = getTile(position);
    const TileDef* def = &getTileDef(tile->type);

    if (def->variantType == VairantType::Random) {
        std::random_device rd;
        std::mt19937 rng{rd()};
        std::uniform_int_distribution<int> varietyDist(0, def->variations - 1);
        tile->variant = varietyDist(rng);
    }
    else if (def->variantType == VairantType::Neighbour) {
        tile->variant = 0;

        for (int i = 0; i < 4; i++) {
            Tile* neighbour = getTile(position + TILE_OFFSETS[i]);
            if (neighbour && tile->type == neighbour->type) {
                tile->variant += (int)std::pow(2, i);
            }

            if (neighbour &&
                getTileDef(neighbour->type).variantType == VairantType::Neighbour) {
                neighbour->variant = 0;
                for (int j = 0; j < 4; j++) {
                    Tile* subNeighbour =
                        getTile(position + TILE_OFFSETS[i] + TILE_OFFSETS[j]);
                    if (subNeighbour && subNeighbour->type == neighbour->type) {
                        neighbour->variant += (int)std::pow(2, j);
                    }
                }
            }
        }
    }

    // Update the tile texture coords
    int vertexIndex = (position.y * m_worldSize + position.x) * 4;
    if (vertexIndex >= (int)m_tileVerts.size()) {
        return;
    }
    sf::Vertex* vertex = &m_tileVerts[vertexIndex];
    float idx = static_cast<float>(def->textureIndex);
    vertex[0].texCoords = {tile->variant * TILE_WIDTH, TILE_HEIGHT * idx};
    vertex[1].texCoords = {tile->variant * TILE_WIDTH, TILE_HEIGHT * (idx + 1)};
    vertex[2].texCoords = {tile->variant * TILE_WIDTH + TILE_WIDTH,
                           TILE_HEIGHT * (idx + 1)};
    vertex[3].texCoords = {tile->variant * TILE_WIDTH + TILE_WIDTH, TILE_HEIGHT * idx};
}

void Map::draw(sf::RenderWindow* target)
{
    sf::RenderStates states = sf::RenderStates::Default;
    states.texture = &m_tileTextures;

    target->draw(m_tileVerts.data(), m_tileVerts.size(), sf::Quads, states);

    if (showDetail) {
        target->draw(m_grid.data(), m_grid.size(), sf::Lines);
    }

    for (const auto& structure : sorted) {
        const auto& str = m_structures[structure];
        const StructureDef* def = &getStructure(str.type);

        m_structureRect.setSize({TILE_WIDTH * def->size.x, TILE_HEIGHT * def->size.y});

        m_structureRect.setTextureRect({(int)TILE_WIDTH * str.variant * (int)def->size.x,
                                        (int)TILE_HEIGHT * def->textureIndex,
                                        (int)def->size.x * (int)TILE_WIDTH,
                                        (int)def->size.y * (int)TILE_HEIGHT});

        m_structureRect.setOrigin({0, m_structureRect.getSize().y - TILE_HEIGHT});
        m_structureRect.setPosition(tileToScreenPosition(m_worldSize, structure));
        target->draw(m_structureRect);
    }
}

void Map::placeStructure(StructureType type, const sf::Vector2i& position)
{
    if (position.y < 0 || position.y >= m_worldSize || position.x < 0 ||
        position.x >= m_worldSize) {
        return;
    }
    if (m_structures.find(position) == m_structures.end()) {
        Structure* structure =
            &m_structures.emplace(std::make_pair(position, Structure{type}))
                 .first->second;
        sorted.insert(position);

        const StructureDef* def = &getStructure(type);

        if (def->variantType == VairantType::Random) {

            std::random_device rd;
            std::mt19937 rng{rd()};
            std::uniform_int_distribution<int> varietyDist(0, def->variations - 1);
            structure->variant = varietyDist(rng);
        }
        else if (def->variantType == VairantType::Neighbour) {
            structure->variant = 0;

            // Update the structure based on its neighbours
            for (int i = 0; i < 4; i++) {
                auto neighbour = m_structures.find(position + TILE_OFFSETS[i]);
                if (neighbour != m_structures.end() &&
                    neighbour->second.type == structure->type) {
                    structure->variant += (int)std::pow(2, i);
                }

                if (neighbour != m_structures.end() &&
                    getStructure(neighbour->second.type).variantType ==
                        VairantType::Neighbour) {
                    neighbour->second.variant = 0;

                    for (int j = 0; j < 4; j++) {
                        auto subNeighbour = m_structures.find(position + TILE_OFFSETS[i] +
                                                              TILE_OFFSETS[j]);

                        if (subNeighbour != m_structures.end() &&
                            subNeighbour->second.type == neighbour->second.type) {
                            neighbour->second.variant += (int)std::pow(2, j);
                        }
                    }
                }
            }
        }
    }
}
*/

void TileChunkManager::initChunks()
{
    tileTextures.loadFromFile("data/Textures/TileMap2.png");
    for (int y = 0; y < 2; y++) {
        for (int x = 0; x < 2; x++) {
            TileChunk chunk;
            chunk.init({x, y}, CHUNK_SIZE);
            chunk.tileTextures = &tileTextures;

            tilechunks.push_back(chunk);
        }
    }
}

void TileChunk::draw(sf::RenderTarget& window, sf::RenderStates states) const
{
    states.texture = tileTextures;
    states.transform *= getTransform();

    window.draw(m_tileVerts.data(), m_tileVerts.size(), sf::Quads, states);
}

void TileChunk::updateTile(const sf::Vector2i& position)
{
    // Set the tile'structure variant
    Tile* tile = getTile(position);
    const TileDef* def = &getTileDef(tile->type);

    if (def->variantType == VairantType::Random) {
        std::random_device rd;
        std::mt19937 rng{rd()};
        std::uniform_int_distribution<int> varietyDist(0, def->variations - 1);
        tile->variant = varietyDist(rng);
    }
    else if (def->variantType == VairantType::Neighbour) {
        tile->variant = 0;

        for (int i = 0; i < 4; i++) {
            Tile* neighbour = getTile(position + TILE_OFFSETS[i]);
            if (neighbour && tile->type == neighbour->type) {
                tile->variant += (int)std::pow(2, i);
            }

            if (neighbour &&
                getTileDef(neighbour->type).variantType == VairantType::Neighbour) {
                neighbour->variant = 0;
                for (int j = 0; j < 4; j++) {
                    Tile* subNeighbour =
                        getTile(position + TILE_OFFSETS[i] + TILE_OFFSETS[j]);
                    if (subNeighbour && subNeighbour->type == neighbour->type) {
                        neighbour->variant += (int)std::pow(2, j);
                    }
                }
            }
        }
    }

    // Update the tile texture coords
    int vertexIndex = (position.y * CHUNK_SIZE + position.x) * 4;
    if (vertexIndex >= (int)m_tileVerts.size()) {
        return;
    }
    sf::Vertex* vertex = &m_tileVerts[vertexIndex];
    float idx = static_cast<float>(def->textureIndex);
    vertex[0].texCoords = {tile->variant * TILE_WIDTH, TILE_HEIGHT * idx};
    vertex[1].texCoords = {tile->variant * TILE_WIDTH, TILE_HEIGHT * (idx + 1)};
    vertex[2].texCoords = {tile->variant * TILE_WIDTH + TILE_WIDTH,
                           TILE_HEIGHT * (idx + 1)};
    vertex[3].texCoords = {tile->variant * TILE_WIDTH + TILE_WIDTH, TILE_HEIGHT * idx};
}

Tile* TileChunk::getTile(const sf::Vector2i& position)
{
    // Check for out of bounds
    static Tile e;
    if (position.y < 0 || position.y >= CHUNK_SIZE || position.x < 0 ||
        position.x >= CHUNK_SIZE) {
        return &e;
    }

    // Get the tile
    return &tiles.at(position.y * CHUNK_SIZE + position.x);
}

void TileChunk::init(const sf::Vector2i& position, int worldSize)
{
    tiles = generateWorld({0, 0}, worldSize);
    chunkPosition = position;

    for (int y = 0; y < worldSize; y++) {
        for (int x = 0; x < worldSize; x++) {
            addIsometricQuad(&m_tileVerts, worldSize, {x, y});
            updateTile({x, y});
        }
    }

    setPosition(tileToScreenPosition(CHUNK_SIZE, {position.x * CHUNK_SIZE, position.y * CHUNK_SIZE}));
}
