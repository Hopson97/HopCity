#include "Map.h"

#include "WorldGeneration.h"
#include <cmath>
#include <iostream>
#include <random>

#include "Common.h"
#include <SFML/Window/Mouse.hpp>

namespace {
    void addIsometricQuad(std::vector<sf::Vertex>* quads,
                          const sf::Vector2i& tilePosition)
    {
        auto pos = tileToScreenPosition(tilePosition);

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

    sf::Vector2i toChunkPosition(const sf::Vector2i& tilePosition)
    {
        return {tilePosition.x / CHUNK_SIZE, tilePosition.y / CHUNK_SIZE};
    }

    sf::Vector2i toLocalTilePosition(const sf::Vector2i& worldTilePosition)
    {
        return {worldTilePosition.x % CHUNK_SIZE, worldTilePosition.y % CHUNK_SIZE};
    }

    // https://gamedevelopment.tutsplus.com/tutorials/how-to-use-tile-bitmasking-to-auto-tile-your-level-layouts--cms-25673
    const sf::Vector2i TILE_OFFSETS[4] = {{0, 1}, {-1, 0}, {1, 0}, {0, -1}};

} // namespace

void TileChunkManager::initWorld()
{
    m_tileTextures.loadFromFile("data/Textures/TileMap2.png");
    m_structureMap.loadFromFile("data/Textures/Structures.png");

    m_structureRect.setTexture(&m_structureMap);

    for (int y = 0; y < 1; y++) {
        for (int x = 0; x < 2; x++) {
            TileChunk chunk;
            chunk.init({x, y}, this);

            m_chunks[{x, y}] = chunk;
        }
    }

    for (int i = 0; i < CHUNK_SIZE + 1; i++) {
        addGridLine(&m_grid, tileToScreenPosition({0, i}),
                    tileToScreenPosition({CHUNK_SIZE, i}));
        addGridLine(&m_grid, tileToScreenPosition({i, 0}),
                    tileToScreenPosition({i, CHUNK_SIZE}));
    }
}

void TileChunkManager::regenerateTerrain()
{
    // m_structures.clear();
    // sorted.clear();
    // m_tiles = generateWorld({0, 0}, m_worldSize, this);
    //
    // for (int y = 0; y < m_worldSize; y++) {
    //    for (int x = 0; x < m_worldSize; x++) {
    //        updateTile({x, y});
    //    }
    //}
}

void TileChunkManager::setTile(const sf::Vector2i& tilePosition, TileType type)
{
    sf::Vector2i chunkPos = toChunkPosition(tilePosition);
    sf::Vector2i localPos = toLocalTilePosition(tilePosition);

    auto chunkItr = m_chunks.find(chunkPos);
    if (chunkItr == m_chunks.end()) {
        return;
    }

    chunkItr->second.getTile(localPos)->type = type;
    chunkItr->second.updateTile(localPos);

    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            sf::Vector2i chunkPos = toChunkPosition(tilePosition + sf::Vector2i{i, j});
            sf::Vector2i localPos =
                toLocalTilePosition(tilePosition + sf::Vector2i{i, j});

            auto chunkItr = m_chunks.find(chunkPos);
            if (chunkItr != m_chunks.end()) {
                chunkItr->second.updateTile(localPos);
            }
        }
    }
}

Tile* TileChunkManager::getTile(const sf::Vector2i& tilePosition)
{
    sf::Vector2i chunkPos = toChunkPosition(tilePosition);
    sf::Vector2i localPos = toLocalTilePosition(tilePosition);

    // Get the tile

    auto chunkItr = m_chunks.find(chunkPos);
    if (chunkItr != m_chunks.end()) {
        return chunkItr->second.getTile(localPos);
    }
    else {
        static Tile t;
        return &t;
    }
}

void TileChunkManager::draw(sf::RenderWindow* window)
{
    sf::RenderStates states = sf::RenderStates::Default;
    states.texture = &m_tileTextures;

    for (auto& chunk : m_chunks) {
        chunk.second.draw(*window, states);
    }
    if (showDetail) {
        window->draw(m_grid.data(), m_grid.size(), sf::Lines);
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
        m_structureRect.setPosition(tileToScreenPosition(structure));
        window->draw(m_structureRect);
    }
}

void TileChunkManager::placeStructure(StructureType type, const sf::Vector2i& position)
{
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

void TileChunk::draw(sf::RenderTarget& window, sf::RenderStates states) const
{
    states.transform *= getTransform();
    window.draw(m_tileVerts.data(), m_tileVerts.size(), sf::Quads, states);
}

void TileChunk::updateTile(const sf::Vector2i& position)
{
    // Set the tile'structure variant
    Tile* tile = getGlobalTile(position);
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
            Tile* neighbour = getGlobalTile(position + TILE_OFFSETS[i]);
            if (neighbour && tile->type == neighbour->type) {
                tile->variant += (int)std::pow(2, i);
            }

            if (neighbour &&
                getTileDef(neighbour->type).variantType == VairantType::Neighbour) {
                neighbour->variant = 0;
                for (int j = 0; j < 4; j++) {
                    Tile* subNeighbour =
                        getGlobalTile(position + TILE_OFFSETS[i] + TILE_OFFSETS[j]);
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
    return &m_tiles.at(position.y * CHUNK_SIZE + position.x);
}

Tile* TileChunk::getGlobalTile(const sf::Vector2i& position)
{
    return mp_chunkManager->getTile({m_chunkPosition.x * CHUNK_SIZE + position.x,
                                     m_chunkPosition.y * CHUNK_SIZE + position.y});
}

//    std::uniform_int_distribution<int> seedDist(0, 4096);

void TileChunk::init(const sf::Vector2i& position, TileChunkManager* chunkManager)
{
    m_tiles = generateWorld(position, 450);
    m_chunkPosition = position;
    mp_chunkManager = chunkManager;

    for (int y = 0; y < CHUNK_SIZE; y++) {
        for (int x = 0; x < CHUNK_SIZE; x++) {
            addIsometricQuad(&m_tileVerts, {x, y});
            updateTile({x, y});
        }
    }

    setPosition(
        tileToScreenPosition({(position.x - 2) * CHUNK_SIZE, position.y * CHUNK_SIZE}));
}
